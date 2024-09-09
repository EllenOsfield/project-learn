#include <cerrno>
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
    vsg::ref_ptr<vsg::AnimationManager> animationManager;
    vsg::ref_ptr<vsg::Group> scenegraph;
    vsg::ref_ptr<vsg::Switch> wonSwitch;
    vsg::ref_ptr<vsg::Animation> wonAnimation;
    double scale = 1.0;
    bool verbose = true;
    int numCorrect = 0;
    int numWrong = 0;
    int sum = 0;

    IntersectionHandler(vsg::ref_ptr<vsg::Camera> in_camera, vsg::ref_ptr<vsg::AnimationManager> in_animationManager, vsg::ref_ptr<vsg::Group> in_scenegraph, int in_sum) :
        camera(in_camera),
        animationManager(in_animationManager),
        scenegraph(in_scenegraph),
        sum(in_sum)
    {

        wonSwitch = scenegraph->getObject<vsg::Switch>("WonSwitch");
        wonAnimation = scenegraph->getObject<vsg::Animation>("WonAnimation");

        vsg::info("WonSwitch = ", wonSwitch);
        vsg::info("WonAnimation = ", wonAnimation);

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

        // sort the intersections front to back
        std::sort(intersector->intersections.begin(), intersector->intersections.end(), [](auto& lhs, auto& rhs) { return lhs->ratio < rhs->ratio; });

        auto& intersection = intersector->intersections.front();
        for (auto& node : intersection->nodePath)
        {
            auto const_sw = node->cast<vsg::Switch>();
            auto sw = const_cast<vsg::Switch*>(const_sw);
            if (sw)
            {
                vsg::uivec2 position;
                uint32_t index;
                uint32_t target;
                node->getValue("position", position);
                node->getValue("index", index);
                node->getValue("target", target);

                bool previousMatching = target == 1 && index == 1;
                bool previousWrong = target != 1 && index == 1;

                // update index to next child
                index = (index + 1) % sw->children.size();
                sw->setValue("index", index);

               // if (index==target) std::cout<<", matching target";

                bool newMatching = target == 1 && index == 1;
                bool newWrong = target != 1 && index == 1;

#if 1
                numCorrect += (previousMatching ? -1 : 0) + (newMatching ? +1 : 0);
                numWrong += (previousWrong ? -1 : 0) + (newWrong ? +1 : 0);
#else
                if (newMatching && !previousMatching) ++numCorrect;
                else if (!newMatching && previousMatching) --numCorrect;

                if (newWrong && !previousWrong) ++numofwrong;
                else if (!newWrong && previousWrong) --numofwrong;

#endif
                sw->setSingleChildOn(index);

                if (numCorrect==sum && numWrong==0)
                {
                    std::cout<<"Well done, you are not a Monica!"<<std::endl;

                    std::cout<<"Finally"<<std::endl;


                    if (wonSwitch)
                    {
                        wonSwitch->setSingleChildOn(0);

                    }
                    if (wonAnimation)
                    {
                        animationManager->play(wonAnimation);
                    }

                }
            }
        }
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

std::vector<int> computeRowCounts(Grid& grid, int row)
{
    std::vector<int> nums;
    // int mus = 0;
    int block = 0;
    int blackblockcounter = 0;
    for(uint32_t column=0; column<grid.width(); ++column)
    {
        if (grid.at(column,row) == 1)
        {
           //  mus += 1,
            block += 1;
        }

        if (block == 1)
            blackblockcounter += 1;
        else
            if (blackblockcounter != 0)
            {
                nums.push_back(blackblockcounter);
                blackblockcounter = 0;
            }
            else
                blackblockcounter = 0;

        block = 0;
    }

    if (blackblockcounter != 0)
    {
        nums.push_back(blackblockcounter);
    }



    return nums;
}

std::vector<int> computeColumnCounts(Grid& grid, int column)
{
    std::vector<int> numbs;
    // int mus = 0;
    int block = 0;
    int blackblockcounter = 0;
    for(uint32_t row=0; row<grid.width(); ++row)
    {
        if (grid.at(column,row) == 1)
        {
           //  mus += 1,
            block += 1;
        }

        if (block == 1)
            blackblockcounter += 1;
        else
            if (blackblockcounter != 0)
            {
                numbs.push_back(blackblockcounter);
                blackblockcounter = 0;
            }
            else
                blackblockcounter = 0;

        block = 0;
    }

    if (blackblockcounter != 0)
    {
        numbs.push_back(blackblockcounter);
    }

    return numbs;
}


int counter(Grid& grid)
{
   int sum = 0 ;

    for(uint32_t row=0; row<grid.height(); ++row)
    {
        for(uint32_t column=0; column<grid.width(); ++column)
        {
            if (grid(column, row)==1) ++sum;
        }
    }
    std::cout<<"total sum ="<<sum<<std::endl;

    return sum;
}

vsg::ref_ptr<vsg::Group> createScene(vsg::ref_ptr<Grid> grid, float spacing, vsg::ref_ptr<vsg::Options> options, bool editingGame )
{

    auto scene = vsg::Group::create();

    auto builder = vsg::Builder::create();
    vsg::GeometryInfo geomInfo;
    vsg::StateInfo stateInfo;

    vsg::vec3 origin(0.0f, 0.0f, 0.0f);

    geomInfo.position = origin;

    scene->setObject("grid", grid);

    for(uint32_t row=0; row<grid->height(); ++row)
    {
        for(uint32_t column=0; column<grid->width(); ++column)
        {
            auto sw = vsg::Switch::create();
            scene->addChild(sw);

            uint32_t target = grid->at(column,row);
            uint32_t index = editingGame ? target : 0;

            sw->setValue("position", vsg::uivec2(column, row));
            sw->setValue("target", target);
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

    // create labels
    {
        vsg::Path font_filename = "fonts/times.vsgb";
        auto font = vsg::read_cast<vsg::Font>(font_filename, options);

        vsg::vec3 position = origin;
        position.x -= 1.0f;

        for(uint32_t row=0; row<grid->height(); ++row)
        {
            auto nums = computeRowCounts(*grid, row);

            std::stringstream label;

            if (nums.size() == 0 )
            {
                label<<"0";
            }
            else
            {
                for(int n : nums)
                {
                    label<<n<<"  ";
                }
            }

            auto layout = vsg::StandardLayout::create();
            layout->glyphLayout = vsg::StandardLayout::LEFT_TO_RIGHT_LAYOUT;
            layout->position = position;
            layout->horizontal = vsg::vec3(0.7, 0.0, 0.0);
            layout->vertical = vsg::vec3(0.0, 0.7, 0.0);
            layout->horizontalAlignment = vsg::StandardLayout::RIGHT_ALIGNMENT;
            layout->verticalAlignment = vsg::StandardLayout::CENTER_ALIGNMENT;
            layout->color = vsg::vec4(0.0, 1.0, 0.0, 1.0);\

            auto text = vsg::Text::create();
            text->text = vsg::stringValue::create(label.str());
            text->font = font;
            text->layout = layout;
            text->setup(0, options);
            scene->addChild(text);

            position.y += spacing;
        }

        position = origin + vsg::vec3(0.0f, (static_cast<float>(grid->height())-0.25f) * spacing, 0.0f);

        for(uint32_t column=0; column<grid->height(); ++column)
        {
            auto numbs = computeColumnCounts(*grid, column);

            vsg::vec3 cursor = position;

            if (numbs.empty()) numbs.push_back(0);

            for(auto n : numbs)
            {
                auto layout = vsg::StandardLayout::create();
                layout->glyphLayout = vsg::StandardLayout::LEFT_TO_RIGHT_LAYOUT;
                layout->position = cursor;
                layout->horizontal = vsg::vec3(0.7, 0.0, 0.0);
                layout->vertical = vsg::vec3(0.0, 0.7, 0.0);
                layout->horizontalAlignment = vsg::StandardLayout::CENTER_ALIGNMENT;
                layout->verticalAlignment = vsg::StandardLayout::BOTTOM_ALIGNMENT;
                layout->color = vsg::vec4(0.0, 1.0, 0.0, 1.0);

                auto text = vsg::Text::create();
                text->text = vsg::stringValue::create(vsg::make_string(n));
                text->font = font;
                text->layout = layout;
                text->setup(0, options);
                scene->addChild(text);

                cursor.y += spacing * 0.9;
            }

            position.x += spacing;
        }
    }

    return scene;
}

int main(int argc, char** argv)
{
    vsg::CommandLine arguments(&argc, argv);

    auto options = vsg::Options::create();
    options->paths = vsg::getEnvPaths("VSG_FILE_PATH");

    // add vsgXchange's support for reading and writing 3rd party file formats
    options->add(vsgXchange::all::create());


    auto dimensions = arguments.value(vsg::uivec2(4,4), "-s");
    auto gridsFilename = arguments.value(vsg::Path("grids.vsgt"), "--grids");
    auto outputFilename = arguments.value(vsg::Path(""), "-o");
    auto editingGame = arguments.read("edit");

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

    auto scene = vsg::Group::create();


    auto board = createScene(grid, spacing, options, editingGame);

    // compute the extents of the board
    vsg::ComputeBounds computeBounds;
    computeBounds.useNodeBounds = false;
    board->accept(computeBounds);
    auto gridBounds = computeBounds.bounds;
    vsg::dvec3 gridCentre = (gridBounds.min + gridBounds.max) * 0.5;
    double gridSize = vsg::length(gridBounds.max - gridBounds.min);

    scene->addChild(board);


    vsg::ref_ptr<vsg::Animation> wonAnimation;
    {
        struct ModelBound
        {
            vsg::ref_ptr<vsg::Node> node;
            vsg::dbox bounds;
        };

        std::list<ModelBound> models;

        vsg::Path filename = "/home/ellen/glTF-Sample-Assets/Models/BrainStem/glTF-Binary/BrainStem.glb";
        if (auto node = vsg::read_cast<vsg::Node>(filename, options))
        {
            vsg::ComputeBounds computeBounds;
            computeBounds.useNodeBounds = false;
            node->accept(computeBounds);
            models.push_back(ModelBound{node, computeBounds.bounds});

            auto diameter = vsg::length(computeBounds.bounds.max - computeBounds.bounds.min);
            vsg::dvec3 origin = (computeBounds.bounds.min + computeBounds.bounds.max) * 0.5;

            auto transform = vsg::MatrixTransform::create();
            transform->matrix = vsg::translate(gridCentre + vsg::dvec3(1.0, 0.0, 6.0))   * vsg::scale(gridSize/diameter) * vsg::rotate(vsg::radians(-90.0), vsg::dvec3(1.0, 0.0, 0.0)) * vsg::translate(-origin);
            transform->addChild(node);

            auto sw = vsg::Switch::create();
            sw->setValue("name", "AnimationSwitch");
            sw->addChild(false, transform);

            scene->addChild(sw);



            // find the animations available in scene
            vsg::FindAnimations findAnimations;
            node->accept(findAnimations);

            auto animations = findAnimations.animations;
            auto animationGroups = findAnimations.animationGroups;

            std::cout << "Model contains " << animations.size() << " animations." << std::endl;
            for (auto& ag : animationGroups)
            {
                std::cout << "AnimationGroup " << ag << std::endl;
                for (auto animation : ag->animations)
                {
                    std::cout << "    animation : " << animation->name << std::endl;
                }
            }

            if (!animations.empty())
            {
                wonAnimation = animations.front();
                scene->setObject("WonSwitch", sw);
                scene->setObject("WonAnimation", wonAnimation);
            }

            vsg::info("loaded ", node);
        }
        else {
            vsg::info("failed to load ", node);

        }
    }


    auto viewer = vsg::Viewer::create();

    auto count = counter(*grid);

    // create window with default traits
    auto windowTraits = vsg::WindowTraits::create();
    auto window = vsg::Window::create(windowTraits);
    viewer->addWindow(window);

    // set up the camera
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
    viewer->addEventHandler(vsg::Trackball::create(camera));

    auto intersectionHandler = IntersectionHandler::create(camera, viewer->animationManager, scene, count);
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



    if (editingGame)
    {
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
    }

    return 0;
}
