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
	// std::string document_type = program.get<std::string>("--type"); // kept for debug purpose
	ocr::document_type type = ocr::type_map[program.get<std::string>("--type")]; // retrieve document type

	std::string document_path = program.get<std::string>("--path"); // retrieve document path
	std::cout << document_path << std::endl;

	// do not process if unknown document type
	if (type == ocr::document_type::unknown)
	{
		std::cerr << "Unknown document type! Please provide a document type to process.\n" << std::endl;
		std::cerr << program;
		std::exit(1);
	}
	//
	if (!std::filesystem::exists(document_path))
	{
		std::cerr << "File: " << document_path << "not found\n" << std::endl;
		std::cerr << program;
		std::exit(1);
	}

	ocr::compute_orientation(document_path);

	nlohmann::json processing_result = {
		{"status", "the status"},
		{"message", "the message"},
		{"data", {{"document-type", "insurance card"}, {"ocr-data", 807}}}
	};
	std::cout << processing_result.dump(4) << '\n';

	ocr::OcrEngine<ocr::StratA>().ocrize("default", "name", 18, ocr::StratA::female);
    ocr::OcrEngine<ocr::StratB>().ocrize("overridden", 18, 3000, 4.5);

	return 0;
}