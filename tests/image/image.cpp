#include "owl/image/image.hpp"
#include "owl/image/image_io.hpp"
#include "catch/catch.hpp"


namespace test
{

  TEST_CASE( "image", "[graphics]" )
  {
    using namespace owl::image;
    using namespace owl::utils;
    std::string path = "../tests/data/images/owl.png";

    rgb8u_image img;
    CHECK(read_image(img, path));

    CHECK(img.width() == 512);
    CHECK(img.height() == 512);
    auto gridimg = create_grid_color(21,21);
    CHECK(write_bmp(gridimg, "images/grid.bmp"));
    CHECK(write_png(gridimg, "images/grid.png"));
    CHECK(write_jpg(gridimg, "images/grid.jpg",96));
    CHECK(read_image(img,"images/grid.bmp"));
    CHECK(img.width() == 101);
    CHECK(img.height() == 101);
    CHECK(img.color(img.pixel(50,50)) == owl::color::rgb8u(255,100,100));
    CHECK(read_image(img,"images/grid.png"));
    CHECK(img.width() == 101);
    CHECK(img.height() == 101);
    CHECK(img.color(img.pixel(50,50)) == owl::color::rgb8u(255,100,100));
    CHECK(read_image(img,"images/grid.jpg"));
    CHECK(img.width() == 101);
    CHECK(img.height() == 101);

  }
}



