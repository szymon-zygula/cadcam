#include "ccwindow.hpp"
#include "viewer.hpp"

#include <functional>

#include <AIS_Shape.hxx>
#include <Aspect_DisplayConnection.hxx>
#include <Aspect_Handle.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepPrimAPI_MakeRevol.hxx>
#include <GC_MakeArcOfCircle.hxx>
#include <GC_MakeSegment.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <IGESControl_Writer.hxx>
#include <STEPControl_Writer.hxx>

TopoDS_Wire build_wire() {
    constexpr double width = 100.;
    constexpr double thickness = 100.;

    // Profile : Define Support Points
    gp_Pnt aPnt1(-width / 2., 0, 0);
    gp_Pnt aPnt2(-width / 2., -thickness / 4., 0);
    gp_Pnt aPnt3(0, -thickness / 2., 0);
    gp_Pnt aPnt4(width / 2., -thickness / 4., 0);
    gp_Pnt aPnt5(width / 2., 0, 0);

    // Profile : Define the Geometry
    Handle(Geom_TrimmedCurve) arc = GC_MakeArcOfCircle(aPnt2, aPnt3, aPnt4);
    Handle(Geom_TrimmedCurve) segment1 = GC_MakeSegment(aPnt1, aPnt2);
    Handle(Geom_TrimmedCurve) segment2 = GC_MakeSegment(aPnt4, aPnt5);
    Handle(Geom_TrimmedCurve) segment3 = GC_MakeSegment(aPnt1, aPnt5);

    // Profile : Define the Topology
    TopoDS_Edge edge1 = BRepBuilderAPI_MakeEdge(segment1);
    TopoDS_Edge edge2 = BRepBuilderAPI_MakeEdge(arc);
    TopoDS_Edge edge3 = BRepBuilderAPI_MakeEdge(segment2);
    TopoDS_Edge edge4 = BRepBuilderAPI_MakeEdge(segment3);
    return BRepBuilderAPI_MakeWire(edge1, edge2, edge3, edge4);
}

TopoDS_Face build_face() {
    TopoDS_Wire wire = build_wire();

    BRepBuilderAPI_MakeWire wire_builder;
    wire_builder.Add(wire);
    TopoDS_Wire wire_profile = wire_builder.Wire();

    // Body : Prism the Profile
    return BRepBuilderAPI_MakeFace(wire_profile);
}

TopoDS_Shape build_extrusion(TopoDS_Shape shape, double height) {
    if (height == 0) {
        return shape;
    }

    gp_Vec aPrismVec(0, 0, height);
    return BRepPrimAPI_MakePrism(shape, aPrismVec);
}

TopoDS_Shape build_rotation(TopoDS_Shape shape, double angle) {
    if (angle == 0) {
        return shape;
    }

    return BRepPrimAPI_MakeRevol(shape, gp::OX(), angle, true);
}

struct Snake {
    TopoDS_Shape base_shape;
    double param;
    std::function<TopoDS_Shape(TopoDS_Shape, double)> make;
    Viewer* viewer;
    double shift;
    size_t idx;
    double delta;

    Snake(Viewer* viewer, double shift, TopoDS_Shape base_shape,
          std::function<TopoDS_Shape(TopoDS_Shape, double)> make, double delta) :
        base_shape(base_shape), make(make), viewer(viewer), param(0), shift(shift), delta(delta) {
        idx = viewer->shapes.size();
        viewer->shapes.push_back(obj());
    }

    void inc_param() {
        param += delta;
        update();
    }
    void zero_param() {
        param = 0;
        update();
    }

    Object obj() {
        gp_Trsf move;
        // clang-format off
        move.SetValues(
            1, 0, 0, shift * 300,
            0, 1, 0, shift * 300,
            0, 0, 1, 0
        );
        // clang-format on
        base_shape.Location(move);
        return Object(make(base_shape, param), 0, 0, 0);
    }

    void update() { viewer->shapes[idx] = Object(obj()); }
};

int main() {
    CCWindow window;
    Viewer viewer(window.x11());

    Snake face_extrusion(&viewer, -1, build_face(), build_extrusion, 10);
    Snake face_rotation(&viewer, -0.5, build_face(), build_rotation, M_PI / 20);
    Snake wire_extrusion(&viewer, 0, build_wire(), build_extrusion, 10);
    Snake wire_rotation(&viewer, 0.5, build_wire(), build_rotation, M_PI / 20);

    viewer.draw();

    while (window.is_running()) {
        window.poll(viewer);

        if (window.is_key_pressed(GLFW_KEY_Q)) {
            face_extrusion.inc_param();
            viewer.draw();
        }

        if (window.is_key_pressed(GLFW_KEY_W)) {
            face_rotation.inc_param();
            viewer.draw();
        }

        if (window.is_key_pressed(GLFW_KEY_E)) {
            wire_extrusion.inc_param();
            viewer.draw();
        }

        if (window.is_key_pressed(GLFW_KEY_R)) {
            wire_rotation.inc_param();
            viewer.draw();
        }

        if (window.is_key_pressed(GLFW_KEY_T)) {
            face_extrusion.zero_param();
            face_rotation.zero_param();
            wire_extrusion.zero_param();
            wire_rotation.zero_param();
            viewer.draw();
        }

        if (window.is_key_pressed(GLFW_KEY_S)) {
            IGESControl_Writer gwriter;
            gwriter.AddShape(viewer.shapes[0].shape);
            gwriter.AddShape(viewer.shapes[1].shape);
            gwriter.AddShape(viewer.shapes[2].shape);
            gwriter.AddShape(viewer.shapes[3].shape);
            gwriter.Write("ksztalty.igs");

            STEPControl_Writer swriter;
            STEPControl_StepModelType mode = STEPControl_ManifoldSolidBrep;
            swriter.Transfer(viewer.shapes[0].shape, mode);
            swriter.Transfer(viewer.shapes[1].shape, mode);
            swriter.Transfer(viewer.shapes[2].shape, mode);
            swriter.Transfer(viewer.shapes[3].shape, mode);
            swriter.Write("ksztalty.stp");
        }
    }
}
