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
            ForwardIt last_kept_it { first },
                last_item_it;

            ++first;

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

            // :TODO: use something like std::last( first, last ) instead of last - 1

            auto initial_range = std::make_pair( first, last - 1 );
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
                result.values[ i ] += interpolation_parameter * ( second.values[ i ] - first.values[ i ] );
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
    }
}
