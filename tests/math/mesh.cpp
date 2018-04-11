#include "owl/math/mesh.hpp"
#include "owl/math/mesh_io.hpp"
#include "owl/math/mesh_triangulation.hpp"
#include "owl/math/mesh_primitives.hpp"
#include "owl/math/physical_properties.hpp"
#include "owl/math/approx.hpp"
#include "owl/utils/stop_watch.hpp"
#include "catch/catch.hpp"


namespace test
{


  TEST_CASE( "add_face_s", "[math]" )
  {
    using namespace owl::math;
    mesh<float> m;
    auto verts = m.add_vertices(7);
    auto f0 = m.add_face(verts[1], verts[2], verts[0]);
    auto f1 = m.add_face(verts[6], verts[0], verts[5]);
    auto f2 = m.add_face(verts[0], verts[3], verts[4]);

    auto f3 = m.add_face(verts[5], verts[0], verts[4]);

    auto f4 = m.add_face(verts[2], verts[3], verts[0]);
    CHECK(m.check() == 0);
  }

   TEST_CASE( "read dragon", "[math]" )
   {
     using namespace owl::math;

     owl::utils::stop_watch s;
     s.start();
     owl::math::mesh<float> m;
     owl::math::read(m, "/Users/skoenig/Downloads/xyzrgb_dragon.ply");

     CHECK(m.check() == 0);
    // owl::math::triangulate_monoton(m);
   //  CHECK(m.check() == 0);
     s.stop();
     std::cout << "reading dragon: " << s.elapsed_time() <<std::endl;
   }

  TEST_CASE( "read raptor", "[math]" )
  {
    using namespace owl::math;
    mesh<float> m;
    owl::math::read(m, "/Users/skoenig/Downloads/178_raptor.off");
    CHECK(m.check() == 0);
  }

  TEST_CASE( "read horse", "[math]" )
  {
    using namespace owl::math;
    mesh<float> m;
    owl::math::read(m, "/Users/skoenig/Downloads/horse.off");

    CHECK(m.check() == 0);
  }

  TEST_CASE( "add_face4", "[math]" )
  {
    using namespace owl::math;
    mesh<float> m;
    owl::io::create_ply_cube_ascii("test.ply");
    owl::math::read_ply(m,"test.ply");
    CHECK(m.num_faces() == 6);
    CHECK(m.check() == 0);
  }

  TEST_CASE( "add_face", "[math]" )
  {
    using namespace owl::math;
    mesh<float> m;
    auto verts = m.add_vertices(9);
    auto f0 = m.add_face(verts[0], verts[1], verts[2]);
    auto f1 = m.add_face(verts[3], verts[4], verts[5]);
    auto f2 = m.add_face(verts[6], verts[7], verts[8]);
    auto f3 = m.add_face(verts[0], verts[3], verts[6]);
    auto f4 = m.add_face(verts[0], verts[6], verts[8]);
    auto f5 = m.add_face(verts[2], verts[4], verts[3], verts[0]);
    CHECK(m.num_faces() == 6);
    CHECK(m.check() == 0);
  }


  TEST_CASE( "add_face2", "[math]" )
  {
    using namespace owl::math;
    mesh<float> m;
    auto verts = m.add_vertices(6);
    auto f0 = m.add_face(verts[0], verts[1], verts[2]);
    auto f1 = m.add_face(verts[1], verts[3], verts[4]);
    auto f2 = m.add_face(verts[2], verts[4], verts[5]);
    auto f3 = m.add_face(verts[1], verts[4], verts[2]);

    CHECK(m.num_faces() == 4);
    CHECK(m.check() == 0);
  }


  TEST_CASE( "primitive create mesh", "[math]" )
  {
    using namespace owl::math;
    physical_properties<float> prop;


    mesh<float> b = create_box<float>();
    mesh<float> b2 = b;
    CHECK(b.num_faces() == 6);
    CHECK(b.num_vertices() == 8);
    CHECK(b.num_edges() == 12);
    CHECK(b.num_halfedges() == 24);
    CHECK(b.is_quad_mesh());
    CHECK(is_closed(b));
    CHECK(b.check() == 0);

    mesh<float> tet = create_tetradedron<float>();
    CHECK(tet.num_faces() == 4);
    CHECK(tet.num_vertices() == 4);
    CHECK(tet.num_edges() == 6);
    CHECK(tet.is_triangle_mesh());
    CHECK(is_closed(tet));
    CHECK(tet.check() == 0);

    auto ico = create_icosaeder<float>();
    CHECK(ico.check() == 0);
    CHECK(ico.num_vertices() == 12);
    CHECK(ico.num_faces() == 20);
    CHECK(ico.num_edges() == 30);
    CHECK(is_closed(ico));
    CHECK(ico.check() == 0);

    auto geo_sphere = create_geodesic_sphere<float>(2, 2);
    CHECK(geo_sphere.num_faces() == 20 * 4 * 4);
    CHECK(is_closed(geo_sphere));
    CHECK(geo_sphere.check() == 0);

    auto sph = create_sphere<float>(2);
    CHECK(sph.num_faces() == 48 * 48);
    CHECK(sph.num_vertices() == 2258);
    CHECK(sph.num_edges() == 4560);
    CHECK(is_closed(sph));
    CHECK(sph.check() == 0);

    auto cylinder = create_cylinder<float>(1, 2, 48, 48);
    CHECK(cylinder.num_faces() == 2400);
    CHECK(cylinder.num_vertices() == 2354);
    CHECK(cylinder.num_edges() == 4752);
    CHECK(is_closed(cylinder));
    CHECK(cylinder.check() == 0);

    auto torus = create_torus<float>(1, 2, 48, 48);
    CHECK(torus.num_faces() == 48 * 48);
    CHECK(torus.num_vertices() == 48 * 48);
    CHECK(torus.num_edges() == 4608);
    CHECK(is_closed(torus));
    CHECK(torus.is_quad_mesh());
    CHECK(num_shells(torus) == 1);
    CHECK(torus.check() == 0);


    auto octaeder = create_octaeder<float>(1);
    CHECK(octaeder.num_faces() == 8);
    CHECK(octaeder.num_vertices() == 6);
    CHECK(octaeder.num_edges() == 12);
    CHECK(is_closed(octaeder));
    CHECK(octaeder.is_triangle_mesh());
    CHECK(octaeder.check() == 0);

    auto disc = create_disk<float>(1, 48);
    CHECK(disc.num_faces() == 48);
    CHECK(disc.num_vertices() == 48 + 1);
    CHECK(disc.num_edges() == 2 * 48);
    CHECK(is_open(disc));
    CHECK(disc.is_triangle_mesh());
    CHECK(disc.check() == 0);

  }
}
