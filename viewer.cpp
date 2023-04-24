#include "viewer.hpp"

#include <AIS_Shape.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Aspect_Handle.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepPrimAPI_MakeBox.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <GC_MakeSegment.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <OpenGl_GraphicDriver.hxx>
#include <V3d_AmbientLight.hxx>
#include <V3d_DirectionalLight.hxx>

Viewer::Viewer(Window handle) {

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

void Viewer::draw() {
    context->EraseAll(true);
    for (auto sh : shapes) {
        Handle(AIS_Shape) shape = new AIS_Shape(sh.shape);
        context->Display(shape, true);
        context->SetDisplayMode(shape, AIS_Shaded, true);
    }
}
