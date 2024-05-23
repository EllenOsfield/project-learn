#include <vsg/all.h>
#include <vsgXchange/all.h>

#include <iostream>

int main(int argc, char** argv)
{
    std::cout<<"Hello World"<<std::endl;

    for(int i=0; i<argc; ++i)
    {
        std::cout<<"    argv["<<i<<"] = "<<argv[i]<<std::endl;
    }


    vsg::CommandLine arguments(&argc, argv);
    if (arguments.read("-n"))
    {
        std::cout<<"Matched n"<<std::endl;

    }

    std::string name;
    if (arguments.read("--name", name))
    {
        std::cout<<"name = "<<name<<std::endl;
    }

    vsg::vec2 position;
    if (arguments.read("-p", position))
    {
        std::cout<<"position = "<<position<<std::endl;
    }

    return 0;
}
