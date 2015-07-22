#include "simplify.hpp"

#include <vector>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

// simplify_radial

TEST_CASE( "simplify_radial_distance: just returns the points if it has only zero, one or two points (1D)", "[simplify_radial]" )
{
    float points[] { 1.0f, 2.0f },
        simplified[] { 1.0f, 2.0f };

    auto get_point_point_square_distance = []( float first, float second ) { auto diff = second - first; return diff * diff; };

    auto new_last = simplify::simplify_radial_distance( points, points + 0, 1.0f, get_point_point_square_distance );
    REQUIRE( new_last == points );

    new_last = simplify::simplify_radial_distance( points, points + 1, 1.0f, get_point_point_square_distance );
    REQUIRE( new_last == points + 1 );
    REQUIRE( std::equal( points, new_last, simplified ) );

    new_last = simplify::simplify_radial_distance( points, points + 2, 1.0f, get_point_point_square_distance );
    REQUIRE( new_last == points + 2 );
    REQUIRE( std::equal( points, new_last, simplified ) );
}

TEST_CASE( "simplify_radial_distance: simplifies points correctly with the given tolerance (keeping both ends) (1D)", "[simplify_radial]" )
{
    float points[] { 0.0f, 0.1f, 0.5f, 0.99f, 1.0f, 1.01f, 1.5f, 2.0f, 2.1f },
        simplified[] { 0.0f, 1.0f, 2.0f, 2.1f };

    auto get_point_point_square_distance = []( float first, float second ) { auto diff = second - first; return diff * diff; };

    auto new_last = simplify::simplify_radial_distance( points, points + sizeof( points ) / sizeof( points[ 0 ] ), 1.0f, get_point_point_square_distance );

    REQUIRE( std::equal( points, new_last, simplified ) );
}

TEST_CASE( "simplify_radial_distance: simplifies points correctly with the given tolerance (keeping both ends) (2D)", "[simplify_radial]" )
{
    using vec2i = simplify::helpers::vect< int, 2 >;

    std::vector< vec2i > points {
            { 0, 0 },
            { 0, 1 },
            { 2, 0 },
            { 2, 2 },
            { 2, 2 },
            { 3, 3 }
        },
        simplified {
            { 0, 0 },
            { 2, 0 },
            { 2, 2 },
            { 3, 3 }
        };

    auto new_last = simplify::simplify_radial_distance( points.begin(), points.end(), 2, &simplify::helpers::get_point_point_square_distance< int, 2 > );
    REQUIRE( std::equal( points.begin(), new_last, simplified.begin() ) );
}

// simplify_douglas_peucker

TEST_CASE( "simplify_douglas_peucker: just returns the points if it has only zero, one or two points (2D)", "[simplify_douglas_peucker]" )
{
    using vec2i = simplify::helpers::vect< int, 2 >;

    std::vector< vec2i > points {
            { 0, 0 },
            { 0, 1 }
        },
        simplified {
            { 0, 0 },
            { 0, 1 }
        };

    auto new_last = simplify::simplify_douglas_peucker( points.begin(), points.begin() + 0, 1, &simplify::helpers::get_point_segment_square_distance< int, 2 > );
    REQUIRE( new_last == points.begin() );

    new_last = simplify::simplify_douglas_peucker( points.begin(), points.begin() + 1, 1, &simplify::helpers::get_point_segment_square_distance< int, 2 > );
    REQUIRE( new_last == points.begin() + 1 );
    REQUIRE( std::equal( points.begin(), new_last, simplified.begin() ) );

    new_last = simplify::simplify_douglas_peucker( points.begin(), points.begin() + 2, 1, &simplify::helpers::get_point_segment_square_distance< int, 2 > );
    REQUIRE( new_last == points.begin() + 2 );
    REQUIRE( std::equal( points.begin(), new_last, simplified.begin() ) );
}

TEST_CASE( "simplify_douglas_peucker: simplifies points correctly with the given tolerance (keeping both ends) (2D)", "[simplify_douglas_peucker]" )
{
    using vec2f = simplify::helpers::vect< float, 2 >;

    std::vector< vec2f > points {
            //                  + = kept
            //                  - = discarded
            //            step: 0 1 2 3
            { -5.0f, 0.0f }, // +      
            { -4.0f, 5.0f }, //     +  
            { -3.0f, 4.5f }, //       -
            { -2.0f, 4.5f }, //       -
            { 0.0f, 5.0f },  //   +    
            { 0.0f, 5.0f },  //     -  
            { 2.5f, 2.5f },  //     -  
            { 5.0f, 0.0f }   // +      
        },
        simplified {
            { -5.0f, 0.0f },
            { -4.0f, 5.0f },
            { 0.0f, 5.0f },
            { 5.0f, 0.0f }
        };

    auto new_last = simplify::simplify_douglas_peucker( points.begin(), points.end(), 1.0f, &simplify::helpers::get_point_segment_square_distance< float, 2 > );
    REQUIRE( std::equal( points.begin(), new_last, simplified.begin() ) );
}

// simplify

TEST_CASE( "simplify: simplifies points correctly with the given tolerance", "[simplify]" )
{
    float points[] {
            224.55f, 250.15f,
            226.91f, 244.19f,
            233.31f, 241.45f,
            234.98f, 236.06f,
            244.21f, 232.76f,
            262.59f, 215.31f,
            267.76f, 213.81f,
            273.57f, 201.84f,
            273.12f, 192.16f,
            277.62f, 189.03f,
            280.36f, 181.41f,
            286.51f, 177.74f,
            292.41f, 159.37f,
            296.91f, 155.64f,
            314.95f, 151.37f,
            319.75f, 145.16f,
            330.33f, 137.57f,
            341.48f, 139.96f,
            369.98f, 137.89f,
            387.39f, 142.51f,
            391.28f, 139.39f,
            409.52f, 141.14f,
            414.82f, 139.75f,
            427.72f, 127.30f,
            439.60f, 119.74f,
            474.93f, 107.87f,
            486.51f, 106.75f,
            489.20f, 109.45f,
            493.79f, 108.63f,
            504.74f, 119.66f,
            512.96f, 122.35f,
            518.63f, 120.89f,
            524.09f, 126.88f,
            529.57f, 127.86f,
            534.21f, 140.93f,
            539.27f, 147.24f,
            567.69f, 148.91f,
            575.25f, 157.26f,
            580.62f, 158.15f,
            601.53f, 156.85f,
            617.74f, 159.86f,
            622.00f, 167.04f,
            629.55f, 194.60f,
            638.90f, 195.61f,
            641.26f, 200.81f,
            651.77f, 204.56f,
            671.55f, 222.55f,
            683.68f, 217.45f,
            695.25f, 219.15f,
            700.64f, 217.98f,
            703.12f, 214.36f,
            712.26f, 215.87f,
            721.49f, 212.81f,
            727.81f, 213.36f,
            729.98f, 208.73f,
            735.32f, 208.20f,
            739.94f, 204.77f,
            769.98f, 208.42f,
            779.60f, 216.87f,
            784.20f, 218.16f,
            800.24f, 214.62f,
            810.53f, 219.73f,
            817.19f, 226.82f,
            820.77f, 236.17f,
            827.23f, 236.16f,
            829.89f, 239.89f,
            851.00f, 248.94f,
            859.88f, 255.49f,
            865.21f, 268.53f,
            857.95f, 280.30f,
            865.48f, 291.45f,
            866.81f, 298.66f,
            864.68f, 302.71f,
            867.79f, 306.17f,
            859.87f, 311.37f,
            860.08f, 314.35f,
            858.29f, 314.94f,
            858.10f, 327.60f,
            854.54f, 335.40f,
            860.92f, 343.00f,
            856.43f, 350.15f,
            851.42f, 352.96f,
            849.84f, 359.59f,
            854.56f, 365.53f,
            849.74f, 370.38f,
            844.09f, 371.89f,
            844.75f, 380.44f,
            841.52f, 383.67f,
            839.57f, 390.40f,
            845.59f, 399.05f,
            848.40f, 407.55f,
            843.71f, 411.30f,
            844.09f, 419.88f,
            839.51f, 432.76f,
            841.33f, 441.04f,
            847.62f, 449.22f,
            847.16f, 458.44f,
            851.38f, 462.79f,
            853.97f, 471.15f,
            866.36f, 480.77f
        },
        simplified[] {
            224.55f, 250.15f,
            267.76f, 213.81f,
            296.91f, 155.64f,
            330.33f, 137.57f,
            409.52f, 141.14f,
            439.60f, 119.74f,
            486.51f, 106.75f,
            529.57f, 127.86f,
            539.27f, 147.24f,
            617.74f, 159.86f,
            629.55f, 194.60f,
            671.55f, 222.55f,
            727.81f, 213.36f,
            739.94f, 204.77f,
            769.98f, 208.42f,
            779.60f, 216.87f,
            800.24f, 214.62f,
            820.77f, 236.17f,
            859.88f, 255.49f,
            865.21f, 268.53f,
            857.95f, 280.30f,
            867.79f, 306.17f,
            859.87f, 311.37f,
            854.54f, 335.40f,
            860.92f, 343.00f,
            849.84f, 359.59f,
            854.56f, 365.53f,
            844.09f, 371.89f,
            839.57f, 390.40f,
            848.40f, 407.55f,
            839.51f, 432.76f,
            853.97f, 471.15f,
            866.36f, 480.77f
        };

    auto new_last = simplify::simplify2f( points, points + sizeof( points ) / sizeof( points[ 0 ] ), 5.0f );
    REQUIRE( std::equal( points, new_last, simplified ) );
}

TEST_CASE( "simplify: just returns the points if it has only one point", "[simplify]" )
{
    int single_point[] { 1, 2 };

    auto new_last = simplify::simplify2i( single_point, single_point + sizeof( single_point ) / sizeof( single_point[ 0 ] ) );
    REQUIRE( new_last == single_point + sizeof( single_point ) / sizeof( single_point[ 0 ] ) );
}

TEST_CASE( "simplify: just returns the points if it has no points", "[simplify]" )
{
    int * empty { nullptr };

    auto new_last = simplify::simplify2i( empty, empty );
    REQUIRE( new_last == empty );
}
