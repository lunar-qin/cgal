#include "MainWindow.h"
#include "Scene.h"
#include <fstream>

#include <CGAL/Surface_mesh_default_triangulation_3.h>
#include <CGAL/Complex_2_in_triangulation_3.h>
#include <CGAL/make_surface_mesh.h>
#include <CGAL/Surface_mesh_default_criteria_3.h>

#include <CGAL/Collisions/AABB_polyhedral_oracle.h>
#include <CGAL/Collisions/AABB_tree.h>

#define CGAL_C2T3_USE_POLYHEDRON
#include <CGAL/IO/Complex_2_in_triangulation_3_file_writer.h>

void MainWindow::on_actionRemeshing_triggered()
{
  if(onePolygonIsSelected())
  {
    QApplication::setOverrideCursor(Qt::WaitCursor);

    int index = getSelectedPolygonIndex();

    // get active polyhedron
    Polyhedron* pMesh = scene->polyhedron(index);

    // remesh
    double init = clock();

    typedef CGAL::Surface_mesh_default_triangulation_3 Tr;
    typedef CGAL::Complex_2_in_triangulation_3<Tr> C2t3;
    typedef Tr::Geom_traits GT;

    Tr tr; // 3D-Delaunay triangulation
    C2t3 c2t3(tr); // 2D-complex in 3D-Delaunay triangulation

    // meshing parameters
    CGAL::Surface_mesh_default_criteria_3<Tr>  
      facets_criteria(25.0, // angular bound
      0.01,  // mesh sizing
      0.001); // approximation error

    // AABB tree
    std::cout << "Build AABB tree...";
    typedef CGAL::AABB_tree<GT,Polyhedron::Facet_handle,Polyhedron> Tree;
    Tree tree;
    tree.build_faces(*pMesh);
    std::cout << "done" << std::endl;

    // input surface
    typedef CGAL::AABB_polyhedral_oracle<Polyhedron,GT> Input_surface;

    // instantiate surface (linked to the AABB tree)
    Input_surface input(&tree);

    // initial point set
    std::cout << "Insert initial point set...";
    unsigned int nb_initial_points = 10;
    Polyhedron::Point_iterator it;
    unsigned int i = 0;
    for(it = pMesh->points_begin();
        it != pMesh->points_end(), i < nb_initial_points;
	it++, i++)
      tr.insert(*it);
    std::cout << "done" << std::endl;

    // remesh
    std::cout << "Remesh...";
    CGAL::make_surface_mesh(c2t3, input, facets_criteria, CGAL::Manifold_tag());
    std::cout << "done (" << tr.number_of_vertices() << " vertices)" << std::endl;

    if(tr.number_of_vertices() > 0)
    {
      // add remesh as new polyhedron
      Polyhedron *pRemesh = new Polyhedron;

      // TODO: dump output to polyhedron

      // for now...
      std::ofstream out("d:\\remesh.off");
      CGAL::output_surface_facets_to_off(out, c2t3);

      // NO IDEA WHY THIS LINE DOES NOT COMPILE
 //     scene->addPolyhedron(pRemesh,
	//tr("%1 (remesh)").arg(scene->polyhedronName(index)),
	//Qt::magenta,
	//scene->isPolyhedronActivated(index),
	//scene->polyhedronRenderingMode(index));
    }

    QApplication::restoreOverrideCursor();
  }
}

/*


int main() {

// defining the surface
std::ifstream file_input("data/triceratops.off");
Polyhedral_surface surface(file_input);

// defining meshing criteria
CGAL::Surface_mesh_default_criteria_3<Tr> 
facets_criteria(30.,  // angular bound
0.5,  // radius bound
0.5); // distance bound
CGAL::Surface_mesh_default_edges_criteria_3<Tr>
edges_criteria(0.5,  // radius bound
0.5); // distance bound

// meshing surface
CGAL::make_piecewise_smooth_surface_mesh(c2t3, surface,
facets_criteria, edges_criteria,
CGAL::Manifold_tag());

std::cout << "Final number of points: " << tr.number_of_vertices() << "\n";
}

*/