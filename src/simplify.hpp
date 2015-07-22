#pragma once

#include <algorithm>
#include <iterator>

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



        template< class T, std::size_t dimension >
        T get_point_point_square_distance(
            const vect< T, dimension > & first,
            const vect< T, dimension > & second
            )
        {
            auto diff = second - first;

            return dot( diff, diff );
        }
    }
}
