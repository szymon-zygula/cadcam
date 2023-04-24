#pragma once

#include <AIS_InteractiveContext.hxx>
#include <TopoDS_Shape.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <Xw_Window.hxx>

struct Object {
    Object(TopoDS_Shape shape, double x, double y, double z) : x(x), y(y), z(z), shape(shape) {}

    TopoDS_Shape shape;
    double x;
    double y;
    double z;
};

struct Viewer {
    Handle(V3d_Viewer) viewer;
    Handle(V3d_View) view;
    Handle(AIS_InteractiveContext) context;
    Handle(Xw_Window) wnt_window;

    std::vector<Object> shapes;

    Viewer(Window handle);

    void draw();
};
