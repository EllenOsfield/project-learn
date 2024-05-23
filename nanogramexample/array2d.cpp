#include <vsg/all.h>
#include <vsgXchange/all.h>

#include <iostream>

using Grid = vsg::Array2D<int>;

void print(Grid& grid)
{
    for(uint32_t row=0; row<grid.height(); ++row)
   {
       for(uint32_t column=0; column<grid.width(); ++column)
       {
           std::cout<<" | "<<grid.at(column, row);
       }
       std::cout<<" |"<<std::endl;
       std::cout<<"  - - - - - - - - "<<std::endl;
    }
    for(uint32_t column=0; column<grid.width(); ++column)
    {
        for(uint32_t row=0; row<grid.height(); ++row)
        {
            std::cout<<"|"<<grid.at(column, row);
        }
        std::cout<<"|"<<std::endl;
    }
}

void set(Grid& grid)
{
    for(uint32_t row=0; row<grid.height(); ++row)
    {
        for(uint32_t column=0; column<grid.width(); ++column)
        {
            grid.set(column, row, 9);
        }
        std::cout<<"woah"<<std::endl;
    }
}

int main(int argc, char** argv)
{
    vsg::CommandLine arguments(&argc, argv);

    auto dimensions = arguments.value(vsg::uivec2(4,4), "-s");

    std::cout<<"size "<<dimensions<<std::endl;

    auto grid = Grid::create(dimensions.x, dimensions.y);

    print(*grid);

    std::cout<<"----------"<<std::endl;

    print(*grid);

    return 0;
}
