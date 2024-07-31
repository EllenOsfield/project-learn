#include <iostream>
#include <vsg/all.h>
#include <vsgXchange/all.h>

using Grid = vsg::Array2D<int>;

void print(Grid& grid)
{
    for(uint32_t row=0
        ; row<grid.height(); ++row)
   {
       for(uint32_t column=0; column<grid.width(); ++column)
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

vsg::ref_ptr<vsg::Node> createScene(Grid& grid)
{
    auto scene = vsg::Group::create();

    auto builder = vsg::Builder::create();
    vsg::GeometryInfo geomInfo;
    vsg::StateInfo stateInfo;

    geomInfo.position.x = 0.0;
    geomInfo.position.y = 0.0;

    for(uint32_t row=0; row<grid.height(); ++row)
    {
        for(uint32_t column=0; column<grid.width(); ++column)
        {
            auto sw = vsg::Switch::create();
            scene->addChild(sw);

            sw->addChild(true, builder->createQuad(geomInfo,stateInfo));
            sw->addChild(true, builder->createSphere(geomInfo,stateInfo));
            sw->addChild(true, builder->createCylinder(geomInfo,stateInfo));

            sw->setSingleChildOn((row+column)%3);

            geomInfo.position.x += 2.0;
        }
        geomInfo.position.x = 0.0;
        geomInfo.position.y += 2.0;
    }

    return scene;
}

int main(int argc, char** argv)
{
    vsg::CommandLine arguments(&argc, argv);

    auto dimensions = arguments.value(vsg::uivec2(4,4), "-s");
    auto outputFilename = arguments.value(vsg::Path("root.vsgt"), "-o");

    std::cout<<"size "<<dimensions<<std::endl;

    auto grid = Grid::create(dimensions.x, dimensions.y);

    print(*grid);


    auto scene = createScene(*grid);

    vsg::write(scene, outputFilename);

    return 0;
}

