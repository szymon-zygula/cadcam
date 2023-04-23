#include "ccwindow.hpp"

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

bool CCWindow::is_key_pressed(int key) {
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

CCWindow::CCWindow() {
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

void CCWindow::poll(Viewer& viewer) {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (is_key_pressed(GLFW_KEY_R)) {
            static int a = 0;
            // Profile : Define Support Points
            gp_Pnt aPnt1(-myWidth / 2., 0, 0);
            gp_Pnt aPnt2(-myWidth / 2., -myThickness / 4., 0);
            gp_Pnt aPnt3(0, -myThickness / 2., 0);
            gp_Pnt aPnt4(myWidth / 2., -myThickness / 4., 0);
            gp_Pnt aPnt5(myWidth / 2., 0, 0);

            // Profile : Define the Geometry
            Handle(Geom_TrimmedCurve) anArcOfCircle = GC_MakeArcOfCircle(aPnt2, aPnt3, aPnt4);
            Handle(Geom_TrimmedCurve) aSegment1 = GC_MakeSegment(aPnt1, aPnt2);
            Handle(Geom_TrimmedCurve) aSegment2 = GC_MakeSegment(aPnt4, aPnt5);

            // Profile : Define the Topology
            TopoDS_Edge anEdge1 = BRepBuilderAPI_MakeEdge(aSegment1);
            TopoDS_Edge anEdge2 = BRepBuilderAPI_MakeEdge(anArcOfCircle);
            TopoDS_Edge anEdge3 = BRepBuilderAPI_MakeEdge(aSegment2);
            TopoDS_Wire aWire = BRepBuilderAPI_MakeWire(anEdge1, anEdge2, anEdge3);

            // Complete Profile
            gp_Ax1 xAxis = gp::OX();
            gp_Trsf aTrsf;

            aTrsf.SetMirror(xAxis);
            BRepBuilderAPI_Transform aBRepTrsf(aWire, aTrsf);
            TopoDS_Shape aMirroredShape = aBRepTrsf.Shape();

            BRepBuilderAPI_MakeWire mkWire;
            mkWire.Add(aWire);
            TopoDS_Wire myWireProfile = mkWire.Wire();

            // Body : Prism the Profile
            TopoDS_Face myFaceProfile = BRepBuilderAPI_MakeFace(myWireProfile);
            TopoDS_Shape myBody;
            if (a == 0) {
                myBody = myFaceProfile;
            }
            else {
                gp_Vec aPrismVec(0, 0, a);
                myBody = BRepPrimAPI_MakePrism(myFaceProfile, aPrismVec);
            }

            viewer.reset();
            viewer.add(myBody);
            viewer.draw();
            a += 10;
        }

        glfwSwapBuffers(window);
    }
}

Window CCWindow::x11() { return glfwGetX11Window(window); }

CCWindow::~CCWindow() {
    glfwDestroyWindow(window);
    glfwTerminate();
}
