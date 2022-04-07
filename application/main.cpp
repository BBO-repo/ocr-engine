#include <filesystem>

#include <argparse.hpp>
#include <json.hpp>

#include "OcrLib.hpp"
#include "OcrEngine.hpp"

int main(int argc, char *argv[])
{
	std::string application_name{"ocrize"}, application_version{"0.0.0"};
	argparse::ArgumentParser program{application_name, application_version};

	// add type of document argument
	program.add_argument("-t", "--type")
		// indicate argument is mandatory and not optional
		.required()
		// add argument documentation
		.help("specify the type of document.\n"
			  "\t\tcard:      insurance card picture\n"
			  "\t\tpdf_diana: dianalabs pdf document\n"
			  "\t\tpdf_uni:   unilabs pdf document\n"
			  "\t\tunknown:   unknown type\n")
		// authorize only set of value
		.action([](const std::string &value)
				{
				static const std::vector<std::string> choices = {"card", "pdf_diana", "pdf_uni","unknown"};
				if (std::find(choices.begin(), choices.end(), value) != choices.end()) {
					return value;
				}
				return std::string{"unknown"}; });

	// add image path argument
	program.add_argument("-p", "--path")
		// indicate argument is mandatory and not optional
		.required()
		// add argument documentation
		.help("the path to the document or image.\n");

	// parse arguments
	try
	{
		program.parse_args(argc, argv);
	}
	catch (const std::runtime_error &err)
	{
		std::cerr << err.what() << std::endl;
		std::cerr << program;
		std::exit(1);
	}

	// validate passed arguments
	ocr::document_type doc_type = ocr::type_map.at(program.get<std::string>("--type")); // retrieve document type
	std::string document_path = program.get<std::string>("--path"); // retrieve document path

	// do not process if unknown document type
	if (doc_type != ocr::document_type::card)
	{
		//std::cerr << "Unknown document type! Please provide a document type to process.\n" << program << std::endl;
		std::cerr << "For the moment only insurance card document type is supported for ocr.\n" << program << std::endl;
		std::exit(1);
	}
	// do not process if document not exist
	if (!std::filesystem::exists(document_path))
	{
		std::cerr << "File: " << document_path << "not found\n" << program << std::endl;
		std::exit(1);
	}

	// use strategy pattern to perform the right ocr according to document type
	ocr::status status{};
	std::string ocr_data{};
	// ocr strategy pattern
	ocr::OcrEngine ocrE{};
	// ocr concrete implementation
	ocr::InsuranceCardOcrizer icOcr{};
    ocrE.set_ocrizer(&icOcr);
    ocrE.ocrize(document_path, status, ocr_data);

	// print the result in the std cout
	nlohmann::json processing_result = {
		{"status", ocr::status_map.at(status)},
		{"data", {{"document-type", "insurance card"}, {"ocr-data", ocr_data}}}};
	
	std::cout << processing_result.dump(4) << '\n';
	
	return 0;
}