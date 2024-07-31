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

      auto viewer = vsg::Viewer::create();

    // create window with default traits
    auto windowTraits = vsg::WindowTraits::create();
    auto window = vsg::Window::create(windowTraits);
    viewer->addWindow(window);

    // set up the camera
    double radius = vsg::WGS_84_RADIUS_EQUATOR;
    double nearFarRatio = 0.001;

    auto lookAt = vsg::LookAt::create(vsg::dvec3(2.0, 2.0, 10.0), vsg::dvec3(2.0, 2.0, 0.0), vsg::dvec3(0.0, 1.0, 0.0));
    auto perspective = vsg::Perspective::create(60.0, static_cast<double>(window->extent2D().width) / static_cast<double>(window->extent2D().height), 0.01, 20.0);
    auto camera = vsg::Camera::create(perspective, lookAt, vsg::ViewportState::create(window->extent2D()));

    // add close handler to respond to pressing the window close window button and pressing escape
    viewer->addEventHandler(vsg::CloseHandler::create(viewer));

    // add a trackball event handler to control the camera view using the mouse
    viewer->addEventHandler(vsg::Trackball::create(camera));

    // create a command graph to render the scene on specified window
    auto commandGraph = vsg::createCommandGraphForView(window, camera, scene);
    viewer->assignRecordAndSubmitTaskAndPresentation({commandGraph});

    // compile all the Vulkan objects and transfer data required to render the scene
    viewer->compile();

//
// Section 3 : execute the frame loop
//
    while (viewer->advanceToNextFrame())
    {
        // pass any events into EventHandlers assigned to the Viewer
        viewer->handleEvents();

        // update the scene graph, such as adding/removing database pager tiles
        viewer->update();

        // record the commands in the scene graph and submit the completed command buffers to the vulkan queue
        viewer->recordAndSubmit();

        // wait for completion of the rendering and present the resulting color buffer to the Window's swap chain.
        viewer->present();
    }

    vsg::write(scene, outputFilename);

    return 0;
}

