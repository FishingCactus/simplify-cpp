#pragma once

#include <algorithm>
#include <iterator>
#include <stack>
#include <vector>

namespace simplify
{
    template< class ForwardIt, class T, class GetPointPointSquareDistance >
    ForwardIt simplify_radial_distance(
        ForwardIt first,
        ForwardIt last,
        T tolerance,
        GetPointPointSquareDistance get_point_point_square_distance
        )
    {
        using VectorReference = typename std::iterator_traits< ForwardIt >::reference;

        static_assert(
            std::is_same<
                typename std::result_of< GetPointPointSquareDistance( VectorReference, VectorReference ) >::type,
                T
                >::value,
            "get_point_point_square_distance return value must match tolerance type"
            );

        if ( std::distance( first, last ) <= 2 )
        {
            return last;
        }
        else
        {
            T square_tolerance { tolerance * tolerance };
            ForwardIt last_kept_it{ first };

            ++first;

            ForwardIt last_item_it{ first };

            for( ForwardIt it = first; it != last; ++it )
            {
                if ( !( get_point_point_square_distance( *it, *last_kept_it ) < square_tolerance ) )
                {
                    *first++ = std::move( *it );
                    last_kept_it = it;
                }

                last_item_it = it;
            }

            if ( last_kept_it != last_item_it )
            {
                *first++ = std::move( *last_item_it );
            }
        }

        return first;
    }

    template< class Iterator >
    Iterator get_last_included(
        Iterator first,
        Iterator last,
        typename std::enable_if<
            std::is_base_of< std::bidirectional_iterator_tag, typename std::iterator_traits<Iterator>::iterator_category >::value
            >::type * = 0
        )
    {
        return last - 1;
    }

    template< class Iterator >
    Iterator get_last_included(
        Iterator first,
        Iterator last,
        typename std::enable_if<
            !std::is_base_of< std::bidirectional_iterator_tag, typename std::iterator_traits<Iterator>::iterator_category >::value
            >::type * = 0
        )
    {
        Iterator last_included;

        for ( auto it = first; it != last; ++it )
        {
            last_included = it;
        }

        return last_included;
    }

    template< class ForwardIt, class T, class GetPointSegmentSquareDistance >
    ForwardIt simplify_douglas_peucker(
        ForwardIt first,
        ForwardIt last,
        T tolerance,
        GetPointSegmentSquareDistance get_point_segment_square_distance
        )
    {
        using VectorReference = typename std::iterator_traits< ForwardIt >::reference;

        static_assert(
            std::is_same<
                typename std::result_of< GetPointSegmentSquareDistance( VectorReference, VectorReference, VectorReference ) >::type,
                T
                >::value,
            "get_point_segment_square_distance return value must match tolerance type"
            );

        if ( std::distance( first, last ) <= 2 )
        {
            return last;
        }
        else
        {
            T square_tolerance { tolerance * tolerance };

            auto initial_range = std::make_pair( first, get_last_included( first, last ) );
            std::stack< decltype( initial_range ) > range_to_process_table;

            range_to_process_table.push( initial_range );

            std::vector< ForwardIt > to_keep_table;
            to_keep_table.push_back( initial_range.first );

            while( !range_to_process_table.empty() )
            {
                auto range = range_to_process_table.top();
                auto maximum = static_cast< T >( 0 );
                ForwardIt current_maximum_it;

                if ( to_keep_table.back() != range.first )
                    to_keep_table.push_back( range.first );

                range_to_process_table.pop();

                for( ForwardIt it = range.first + 1; it != range.second; ++ it )
                {
                    auto square_distance = get_point_segment_square_distance( *it, *range.first, *range.second );

                    if ( square_distance > maximum )
                    {
                        maximum = square_distance;
                        current_maximum_it = it;
                    }
                }

                if ( maximum >= square_tolerance )
                {
                    range_to_process_table.push( std::make_pair( current_maximum_it, range.second ) );
                    range_to_process_table.push( std::make_pair( range.first, current_maximum_it ) );
                }
            }

            to_keep_table.push_back( initial_range.second );

            for( auto it = to_keep_table.begin(); it != to_keep_table.end(); ++it )
            {
                *first++ = std::move( **it );
            }

            return first;
        }
    }

    template< class ForwardIt, class T, class GetPointSegmentSquareDistance >
    ForwardIt simplify(
        ForwardIt first,
        ForwardIt last,
        T tolerance,
        GetPointSegmentSquareDistance get_point_segment_square_distance
        )
    {
        return simplify_douglas_peucker( first, last, tolerance, get_point_segment_square_distance );
    }

    template< class ForwardIt, class T, class GetPointSegmentSquareDistance, class GetPointPointSquareDistance >
    ForwardIt simplify(
        ForwardIt first,
        ForwardIt last,
        T tolerance,
        GetPointSegmentSquareDistance get_point_segment_square_distance,
        GetPointPointSquareDistance get_point_point_square_distance
        )
    {
        last = simplify_radial_distance( first, last, tolerance, get_point_point_square_distance );

        return simplify_douglas_peucker( first, last, tolerance, get_point_segment_square_distance );
    }

    namespace helpers
    {
        template< class T, std::size_t dimension >
        struct vect
        {
            T values[ dimension ];
        };

        template< class T, std::size_t dimension >
        bool operator==(
            const vect< T, dimension > & first,
            const vect< T, dimension > & second
            )
        {
            for ( std::size_t i = 0; i < dimension; ++i )
            {
                if ( first.values[ i ] != second.values[ i ] )
                {
                    return false;
                }
            }

            return true;
        }

        template< class T, std::size_t dimension >
        vect< T, dimension > operator-(
            const vect< T, dimension > & first,
            const vect< T, dimension > & second
            )
        {
            vect< T, dimension > result;

            for ( std::size_t i = 0; i < dimension; ++i )
            {
                result.values[ i ] = first.values[ i ] - second.values[ i ];
            }

            return result;
        }

        template< class T, std::size_t dimension >
        T dot(
            const vect< T, dimension > & first,
            const vect< T, dimension > & second
            )
        {
            auto result = static_cast< T >( 0 );

            // :TODO: check valid range for T

            for ( std::size_t i = 0; i < dimension; ++i )
            {
                result += first.values[ i ] * second.values[ i ];
            }

            return result;
        }

        template< class T, class P, std::size_t dimension >
        vect< T, dimension > lerp(
            const vect< T, dimension > & first,
            const vect< T, dimension > & second,
            const P interpolation_parameter
            )
        {
            auto result = first;

            for ( std::size_t i = 0; i < dimension; ++i )
            {
                result.values[ i ] += T( interpolation_parameter * ( second.values[ i ] - first.values[ i ] ) );
            }

            return result;
        }

        template< class T, std::size_t dimension >
        T get_point_point_square_distance(
            const vect< T, dimension > & first,
            const vect< T, dimension > & second
            )
        {
            auto diff = second - first;

            return dot( diff, diff );
        }

        template< class T, std::size_t dimension >
        T get_point_segment_square_distance(
            const vect< T, dimension > & candidate,
            const vect< T, dimension > & segment_start,
            const vect< T, dimension > & segment_end
            )
        {
            const auto segment_square_length = get_point_point_square_distance( segment_start, segment_end );

            // :TODO: use precision

            if ( segment_square_length == static_cast< decltype( segment_square_length ) >( 0 ) )
            {
                return get_point_point_square_distance( candidate, segment_start );
            }

            const double t = double( dot( candidate - segment_start, segment_end - segment_start ) ) / segment_square_length;

            if ( t < 0.0 )
            {
                return get_point_point_square_distance( candidate, segment_start );
            }
            else if ( t > 1.0 )
            {
                return get_point_point_square_distance( candidate, segment_end );
            }
            else
            {
                const vect< T, dimension > projection = lerp( segment_start, segment_end, t );

                return get_point_point_square_distance( candidate, projection );
            }
        }

        template< class T, std::size_t dimension >
        T * simplify(
            T * const first,
            T * const last,
            const T tolerance = static_cast< T >( 1 ),
            const bool highest_quality = false
            )
        {
            static_assert( std::is_arithmetic< T >::value, "T is not an arithmetic type" );

            if ( highest_quality )
            {
                return ( T* ) ::simplify::simplify(
                    reinterpret_cast< vect< T, dimension > * >( first ),
                    reinterpret_cast< vect< T, dimension > * >( last ),
                    tolerance,
                    &get_point_segment_square_distance< T, dimension >
                    );
            }
            else
            {
                return ( T* ) ::simplify::simplify(
                    reinterpret_cast< vect< T, dimension > * >( first ),
                    reinterpret_cast< vect< T, dimension > * >( last ),
                    tolerance,
                    &get_point_segment_square_distance< T, dimension >,
                    &get_point_point_square_distance< T, dimension >
                    );
            }
        }
    }

    #define simplify2i helpers::simplify< int, 2 >
    #define simplify3i helpers::simplify< int, 3 >
    #define simplify2f helpers::simplify< float, 2 >
    #define simplify3f helpers::simplify< float, 3 >
    #define simplify2d helpers::simplify< double, 2 >
    #define simplify3d helpers::simplify< double, 3 >
}
