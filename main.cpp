#include <iostream>
#include "common.h"
#include "Image.h"
#include "Offset.h"
#include "Stacker.h"
#include <string>
#include <vector>
#include <boost/program_options.hpp>

namespace po = boost::program_options;


void process_input(const char * source, Stacker * stacker, float threshold) {
    std::cout << "Processing " << source << std::endl;
    Image * image = Image::open(source);
    Offset * offset = image->calculateSimpleCenterOffset(threshold);

    std::cout << "Offset X: " << offset->horiz << ", Y: " << offset->vert << std::endl;

    image->shiftImageCenter(offset);

    stacker->add(image);

    destroy_offset(offset);
    delete(image);
}

int main(int argc, const char *argv[]) {

    po::options_description desc("Options");
    desc.add_options()
            ("help,h", "Help screen")
            ("input,i", po::value<std::vector<std::string>>()->multitoken()->composing())
            ("output,o", po::value<std::string>())
            ("threshold,t", po::value<float>()->default_value(0.1))
            ;

    po::variables_map vm;
    po::store(parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if(!vm.count("output")) {
        desc.print(std::cout);
        exit(1);
    }
    if(!vm.count("input")) {
        desc.print(std::cout);
        exit(1);
    }


    std::string output = vm["output"].as<std::string>();
    std::cout << "Output Path: " << output << std::endl;

    float threshold = vm["threshold"].as<float>();
    std::cout << "Centering Threshold: " << threshold << std::endl;


    Stacker * stacker = new Stacker(5184, 3456);

    if (vm.count("input")) {
        std::vector<std::string> m = vm["input"].as< std::vector<std::string> >();
        std::cout << "Number of input files: " << m.size() << std::endl;
        for (auto i = m.begin(); i != m.end(); ++i) {
            std::cout << *i << std::endl;
            process_input(i->c_str(), stacker, threshold);
        }
    }

    std::cout << "Saving stack image to " << output << std::endl;
    stacker->finalize();
    stacker->save(output.c_str(), 100);

    /*
     *     Image * image = open_image("/Users/kgill/data/Moon/IMG_9954.JPG");

    Offset * offset = calculate_simple_center_offset(image, 10);

    printf("Offset X: %f, Y: %f\n", offset->horiz, offset->vert);

    shift_image_center(image, offset);

    save_image(image, "/Users/kgill/data/Moon/test.jpg", 100);

    destroy_offset(offset);
    destroy_image(image);
     */


    return 0;
}
