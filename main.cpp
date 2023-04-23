#include <unordered_map>

#include <AIS_InteractiveContext.hxx>
#include <AIS_Shape.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Aspect_Handle.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <V3d_AmbientLight.hxx>
#include <V3d_DirectionalLight.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <Xw_Window.hxx>

#include <X11/Xlib.h>

#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

struct Viewer {
    Handle(V3d_Viewer) viewer;
    Handle(V3d_View) view;
    Handle(AIS_InteractiveContext) context;
    Handle(Xw_Window) wnt_window;

    std::vector<TopoDS_Shape> shapes;

    Viewer(Window handle) {

        Handle(Aspect_DisplayConnection) display_connection = new Aspect_DisplayConnection();

        Handle(OpenGl_GraphicDriver) graphic_driver =
            new OpenGl_GraphicDriver(display_connection, false);

        viewer = new V3d_Viewer(graphic_driver);

        Handle(V3d_DirectionalLight) light_dir =
            new V3d_DirectionalLight(V3d_Zneg, Quantity_Color(Quantity_NOC_GRAY97), 1);
        Handle(V3d_AmbientLight) light_amb = new V3d_AmbientLight();

        light_dir->SetDirection(1.0, -2.0, -10.0);
        viewer->AddLight(light_dir);
        viewer->AddLight(light_amb);
        viewer->SetLightOn(light_dir);
        viewer->SetLightOn(light_amb);

        context = new AIS_InteractiveContext(viewer);

        // Configure some global props.
        const Handle(Prs3d_Drawer)& contextDrawer = context->DefaultDrawer();

        if (!contextDrawer.IsNull()) {
            const Handle(Prs3d_ShadingAspect)& SA = contextDrawer->ShadingAspect();
            const Handle(Graphic3d_AspectFillArea3d)& FA = SA->Aspect();
            contextDrawer->SetFaceBoundaryDraw(true); // Draw edges.
            FA->SetEdgeOff();

            // Fix for inifinite lines has been reduced to 1000 from its default value 500000.
            contextDrawer->SetMaximalParameterValue(1000);
        }

        // Main view creation.
        view = viewer->CreateView();
        view->SetImmediateUpdate(false);

        // evt_mgr = new ViewerInteractor(view, context); // TODO TODO TODO

        wnt_window = new Xw_Window(display_connection, handle);
        view->SetWindow(wnt_window, nullptr);
        if (!wnt_window->IsMapped()) {
            wnt_window->Map();
        }
        view->MustBeResized();

        view->SetShadingModel(V3d_PHONG);

        Graphic3d_RenderingParams& RenderParams = view->ChangeRenderingParams();
        RenderParams.IsAntialiasingEnabled = true;
        RenderParams.NbMsaaSamples = 8;
        RenderParams.IsShadowEnabled = false;
        RenderParams.CollectedStats = Graphic3d_RenderingParams::PerfCounters_NONE;
    }

    void add(TopoDS_Shape shape) { shapes.push_back(shape); }

    void start() {
        for (auto sh : shapes) {
            Handle(AIS_Shape) shape = new AIS_Shape(sh);
            context->Display(shape, true);
            context->SetDisplayMode(shape, AIS_Shaded, true);
        }
    }
};

struct CCWindow {
    GLFWwindow* window;
    std::unordered_map<int, bool> down;

    bool is_key_pressed(int key) {
        if (glfwGetKey(window, key) == GLFW_PRESS) {
            if (!down[key]) {
                down[key] = true;
                return true;
            }
        }

        if (glfwGetKey(window, key) == GLFW_RELEASE) {
            down[key] = false;
        }

        return false;
    }

    CCWindow() {
        if (!glfwInit()) {
            std::cerr << "Could not open GLFW window" << std::endl;
            exit(1);
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(1280, 720, "CAD/CAM", NULL, NULL);
        if (window == NULL) {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            exit(1);
        }
        glfwMakeContextCurrent(window);
    }

    void poll(Viewer& view) {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            if(is_key_pressed(GLFW_KEY_R)) {
            }
 
            glfwSwapBuffers(window);
        }
    }

    Window x11() { return glfwGetX11Window(window); }

    ~CCWindow() {
        glfwDestroyWindow(window);
        glfwTerminate();
    }
};

int main() {

    BRepPrimAPI_MakeBox box(100., 200., 300);
    const TopoDS_Shape& shape = box.Shape();

    CCWindow window;
    Viewer viewer(window.x11());

    viewer.add(shape);
    viewer.start();

    window.poll(viewer);
}
