#include "RayTracing.h"
#include "Trace.h"
#include "Graphics.h"
#include "Algebra.h"

using namespace UI;
using namespace UI::Core;
using namespace UI::Base;
using namespace UI::Controls;


class Page0 : public Page
{
private:
    Renderer w;
public:
    void PersetWorld()
    {
        w.cameras.push_back(std::shared_ptr<Camera>(new Camera(Vec3d(0, 0, 500), Vec3d(0, 0, 0), Vec3d(1, 0, 0), Vec3d(0, 1, 0))));
        w.scene = std::shared_ptr<Scene>(new Scene());
        w.scene->ambient = Vec3d(0.1, 0.1, 0.1);

        PointLight* l = new PointLight[2];

        l[0].centre = Vec3d(-500, -50, -400);
        l[0].intensity = Vec3d(0.4, 0.4, 0.4);
        l[1].centre = Vec3d(300, -50, -400);
        l[1].intensity = Vec3d(0.5, 0.5, 0.5);
        w.scene->point_lights.push_back(l);
        w.scene->point_lights.push_back(l + 1);

        TPolygon * p = new TPolygon;
        p->vertices.push_back(Vec3d(-300, 130, 300));
        p->vertices.push_back(Vec3d(300, 130, 300));
        p->vertices.push_back(Vec3d(300, 130, 0));
        p->vertices.push_back(Vec3d(-300, 130, 0));
        p->vertices.push_back(Vec3d(-300, 130, 300));
        p->inormal = Vec3d(0, 0, 0);
        p->material.ambient = Vec3d(0.6, 0.6, 0.6);
        p->material.diffuse = Vec3d(0.6, 0.6, 0.6);
        p->material.specular = 0.9;
        p->material.exponent = 30;
        p->material.reflect = 0.3;
        w.scene->objects.push_back(p);

        Sphere *s1 = new Sphere;
        s1->radius = 75;
        s1->centre = Vec3d(-100, -70, 500);
        s1->material.ambient = Vec3d(1, 0.5, 0);
        s1->material.diffuse = Vec3d(1, 0.5, 0);
        s1->material.specular = 0.9;
        s1->material.exponent = 30;
        s1->material.reflect = 0.4;
        w.scene->objects.push_back(s1);

        Sphere *s2 = new Sphere;
        s2->radius = 75;
        s2->centre = Vec3d(90, 55, 120);
        s2->material.ambient = Vec3d(1, 0, 0);
        s2->material.diffuse = Vec3d(1, 0, 0);
        s2->material.specular = 0.9;
        s2->material.exponent = 30;
        s2->material.reflect = 0.4;
        w.scene->objects.push_back(s2);

        Sphere *s3 = new Sphere;
        s3->radius = 75;
        s3->centre = Vec3d(-90, 55, 120);
        s3->material.ambient = Vec3d(0, 1, 1);
        s3->material.diffuse = Vec3d(0, 1, 1);
        s3->material.specular = 0.6;
        s3->material.exponent = 30;
        s3->material.reflect = 0.3;
        w.scene->objects.push_back(s3);
    }
    
    Page0() : Page()
    {
        PersetWorld();
        w.init();
        content = std::make_shared<Grid>();
        std::shared_ptr<GLContext> context = std::make_shared(GLContext("MainContext", UI::Core::Margin::StretchStretch(0, 0, 0, 0, 0, 0, 0, 0)));
        context->onRenderF = [this]()
        {

        };
    }
};

class Window0 : public Core::Window
{
public:
    Window0() : Core::Window("1", 400, 400, 400, 400)
    {
        pushPage(std::static_pointer_cast<UI::Core::Page>(std::make_shared<Page0>()));
    }
};

class App : public Core::Application
{
public:
    void afterLaunch() override
    {
        addWindow(std::static_pointer_cast<Window>(std::make_shared<Window0>()));
    }
};


//////////////////////////////////////////////////////////////////////////////////
/**********************************************************\
 * Rendering the bitmap into the window.                  *
\**********************************************************/
void app_main(void)                         /* rendering loop */
{
    w.capture(HW_SCREEN_X_SIZE, HW_SCREEN_Y_SIZE, 0);
}

