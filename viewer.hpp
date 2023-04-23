#pragma once

#include <AIS_InteractiveContext.hxx>
#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>
#include <Xw_Window.hxx>
#include <TopoDS_Shape.hxx>

struct Viewer {
    Handle(V3d_Viewer) viewer;
    Handle(V3d_View) view;
    Handle(AIS_InteractiveContext) context;
    Handle(Xw_Window) wnt_window;

    std::vector<TopoDS_Shape> shapes;

    Viewer(Window handle);

    void add(TopoDS_Shape shape);

    void draw();

    void reset();
};
