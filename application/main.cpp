#include <argparse.hpp>

#include "OcrEngine.hpp" 

int main(int argc, char *argv[]) {
  std::string application_name = "ocrize";
  std::string application_version = "0.0.0";
  argparse::ArgumentParser program(application_name, application_version);

  // add type of document argument
  program.add_argument("-t", "--type").required()
  .help("specify the type of document.\n"
  "\t\tcard:      insurance card picture\n"
  "\t\tpdf_diana: dianalabs pdf document\n"
  "\t\tpdf_uni:   unilabs pdf document\n"
  "\t\tunknown:   unknown type\n");

  // add image path argument
  program.add_argument("-p", "--path").required()
  .help("the path to the document or image.\n");

  try {
    program.parse_args(argc, argv);
  }
  catch (const std::runtime_error& err) {
    std::cerr << err.what() << std::endl;
    std::cerr << program;
    std::exit(1);
  }

  std::string document_type = program.get<std::string>("--type");
  std::cout << document_type << std::endl;

  std::string document_path = program.get<std::string>("--path");
  std::cout << document_path << std::endl;

  ocr::compute_orientation(document_path);

  return 0;
}