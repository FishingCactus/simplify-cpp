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
