#include <iostream>
#include <vsg/all.h>
#include <vsgXchange/all.h>

using Grid = vsg::Array2D<int>;

void print(Grid& grid)
{
    for(int32_t row=grid.height()-1; row>=0 ; --row)
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
            grid.set(column, row, (column+row)%3);
        }
        std::cout<<"woah"<<std::endl;
    }
}


class IntersectionHandler : public vsg::Inherit<vsg::Visitor, IntersectionHandler>
{
public:
    vsg::ref_ptr<vsg::Camera> camera;
    vsg::ref_ptr<vsg::Group> scenegraph;
    double scale = 1.0;
    bool verbose = true;

    IntersectionHandler(vsg::ref_ptr<vsg::Camera> in_camera, vsg::ref_ptr<vsg::Group> in_scenegraph) :
        camera(in_camera),
        scenegraph(in_scenegraph)
    {
    }

    void apply(vsg::KeyPressEvent& keyPress) override
    {
    }

#if 0
    void apply(vsg::MoveEvent& moveEvent) override
    {
        if (moveEvent.mask != 0) select(moveEvent);
    }
#endif

    void apply(vsg::ButtonPressEvent& buttonPress) override
    {
        select(buttonPress);
    }

    void select(vsg::PointerEvent& pointerEvent)
    {
        auto intersector = vsg::LineSegmentIntersector::create(*camera, pointerEvent.x, pointerEvent.y);
        scenegraph->accept(*intersector);

        if (intersector->intersections.empty()) return;

        if (verbose) std::cout << "intersection(" << pointerEvent.x << ", " << pointerEvent.y << ") " << intersector->intersections.size() << ") ";

        // sort the intersections front to back
        std::sort(intersector->intersections.begin(), intersector->intersections.end(), [](auto& lhs, auto& rhs) { return lhs->ratio < rhs->ratio; });

        auto& intersection = intersector->intersections.front();
        for (auto& node : intersection->nodePath)
        {
            std::cout << ", " << node->className();

            auto const_sw = node->cast<vsg::Switch>();
            auto sw = const_cast<vsg::Switch*>(const_sw);
            if (sw)
            {
                vsg::uivec2 position;
                uint32_t index;
                if (node->getValue("position", position) && node->getValue("index", index))
                {
                    std::cout<<": position = {"<<position<<"} index = "<<index;
                }

                // update index to next child
                index = (index + 1) % sw->children.size();
                sw->setValue("index", index);

                sw->setSingleChildOn(index);
            }
        }

        std::cout<<std::endl;
    }

protected:
};

class UpdateGrid : public vsg::Visitor
{
public:

    vsg::ref_ptr<Grid> grid;

    void apply (vsg::Object& object) override
    {
        object.traverse(*this);
    }

    void apply (vsg::Group& group) override
    {
        auto local_grid = group.getObject<Grid>("grid");
        if (local_grid) grid = local_grid;

        group.traverse(*this);
    }

    void apply(vsg::Switch& sw) override
    {
        vsg::uivec2 position;
        uint32_t index;
        if (grid && sw.getValue("position", position) && sw.getValue("index", index))
        {
            grid->set(position.x, position.y, index);
        }
    }

};

int counter(Grid& grid)
{
   int sum = 0 ;
   int mus = 0;
   int ums = 0;
   int msu = 0;
   int rtimes = 0;
   int ctimes = 0;
   int num = {};

    for(uint32_t row=0; row<grid.height(); ++row)
        {
            for(uint32_t column=0; column<grid.width(); ++column)
            {
                if (grid.at(column,row) == 1)
                    sum += 1,
                    mus += 1,
                    ums += 1;
                ctimes +=1;
                if (ums == 1)
                    msu += 1;
                else
                    if (msu != 0)
                        num = msu,
                        std::cout<<"num "<<num<<std::endl,
                        msu = 0;
                    else
                        msu = 0;

               // std::cout<<"column "<<ctimes<<" sum = "<<ums<<" and current run  = "<<msu<<std::endl;
                ums = 0;
            }
            msu = 0;
            rtimes += 1;
            std::cout<<"row "<<rtimes<<" sum = "<<mus<<std::endl;
            mus = 0;

        }
    std::cout<<"total sum ="<<sum<<std::endl;

    return sum;
}



vsg::ref_ptr<vsg::Group> createScene(vsg::ref_ptr<Grid> grid, float spacing)
{
    auto scene = vsg::Group::create();

    auto builder = vsg::Builder::create();
    vsg::GeometryInfo geomInfo;
    vsg::StateInfo stateInfo;

    geomInfo.position.x = 0.0;
    geomInfo.position.y = 0.0;

    scene->setObject("grid", grid);

    for(uint32_t row=0; row<grid->height(); ++row)
    {
        for(uint32_t column=0; column<grid->width(); ++column)
        {
            auto sw = vsg::Switch::create();
            scene->addChild(sw);

            uint32_t index = grid->at(column,row);

            sw->setValue("position", vsg::uivec2(column, row));
            sw->setValue("index", index);

            geomInfo.color.set(1.0f, 0.0f, 0.0f, 1.0f);;
            sw->addChild(true, builder->createQuad(geomInfo,stateInfo));

            geomInfo.color.set(0.0f, 0.0f, 0.0f, 0.0f);;
            sw->addChild(true, builder->createQuad(geomInfo,stateInfo));

            geomInfo.color.set(1.0f, 1.0f, 1.0f, 1.0f);;
            sw->addChild(true, builder->createQuad(geomInfo,stateInfo));

            sw->setSingleChildOn(index);


            geomInfo.position.x += spacing;
        }
        geomInfo.position.x = 0.0;
        geomInfo.position.y += spacing;

    }

    return scene;
}

int main(int argc, char** argv)
{
    vsg::CommandLine arguments(&argc, argv);

    auto dimensions = arguments.value(vsg::uivec2(4,4), "-s");
    auto gridsFilename = arguments.value(vsg::Path("grids.vsgt"), "--grids");
    auto outputFilename = arguments.value(vsg::Path(""), "-o");

    std::cout<<"size "<<dimensions<<std::endl;



    // read or create the grids container
    auto grids = vsg::read_cast<vsg::Objects>(gridsFilename);
    if (!grids) grids = vsg::Objects::create();

    // read any individual grid files and add them to grids
    vsg::Path gridFilename;
    while (arguments.read("-g", gridFilename))
    {
        auto grid = vsg::read_cast<Grid>(gridFilename);
        if (!grid) grids->addChild(grid);
    }

    // if grids is empty then add a grid
    if (grids->children.empty())
    {
        auto grid = Grid::create(dimensions.x, dimensions.y);
        grids->addChild(grid);
    }

    vsg::ref_ptr<Grid> grid = grids->children.front().cast<Grid>();
    if (!grid) Grid::create(dimensions.x, dimensions.y);

    print(*grid);

    float spacing = 1.1;
    auto scene = createScene(grid, spacing);

    auto viewer = vsg::Viewer::create();

    auto count = counter(*grid);

    // create window with default traits
    auto windowTraits = vsg::WindowTraits::create();
    auto window = vsg::Window::create(windowTraits);
    viewer->addWindow(window);

    // set up the camera
    vsg::ComputeBounds computeBounds;
    scene->accept(computeBounds);
    double radius = vsg::length(computeBounds.bounds.max - computeBounds.bounds.min) * 0.6;
    double nearFarRatio = 0.001;

    vsg::dvec3 centre = (computeBounds.bounds.min + computeBounds.bounds.max) * 0.5;
    vsg::dvec3 eye = centre + vsg::dvec3(0.0, 0.0, radius * 3.5);

    // set up the camera
    auto viewport = vsg::ViewportState::create(window->extent2D());
    auto perspective = vsg::Perspective::create(30.0, static_cast<double>(window->extent2D().width) / static_cast<double>(window->extent2D().height), nearFarRatio * radius, radius * 1000.0);
    auto lookAt = vsg::LookAt::create(eye, centre, vsg::dvec3(0.0, 1.0, 0.0));
    auto camera = vsg::Camera::create(perspective, lookAt, viewport);

    // add close handler to respond to pressing the window close window button and pressing escape
    viewer->addEventHandler(vsg::CloseHandler::create(viewer));

    // add a trackball event handler to control the camera view using the mouse
    // viewer->addEventHandler(vsg::Trackball::create(camera));

    auto intersectionHandler = IntersectionHandler::create(camera, scene);
    viewer->addEventHandler(intersectionHandler);

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


    UpdateGrid ug;
    scene-> accept(ug);

    count = counter(*grid);

    if (gridsFilename)
    {
        vsg::write(grids, gridsFilename);
    }

    if (outputFilename)
    {
        vsg::write(scene, outputFilename);
    }

    return 0;
}
