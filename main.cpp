#include <BRepPrimAPI_MakeBox.hxx>

int main() {
    BRepPrimAPI_MakeBox mkBox(1., 2., 3);
    const TopoDS_Shape& shape = mkBox.Shape();
}
