#ifndef MORPHY_ROWS_CLASS_H
#define MORPHY_ROWS_CLASS_H

#include <godot_cpp/variant/variant.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

namespace morphy
{

template <uint64_t Z>
class Rows
{
    // There should be at least 2 rows
    static_assert(Z >= 2);

public:
    uint64_t num_columns() const
    {
        return column_count;
    }

    uint64_t num_rows() const
    {
        return Z;
    }

    uint64_t row_capacity() const
    {
        return buffer.size() / Z;
    }

    void add_columns(uint64_t p_num)
    {
        ERR_FAIL_COND(p_num < 0);

        const uint64_t available_capacity = row_capacity();
        const uint64_t requested_capacity = column_count + p_num;
        if (available_capacity < requested_capacity)
        {
            // Use temporary buffer to reduce a wasted copy by resizing the existing buffer
            std::vector<godot::Variant> temp_buffer(requested_capacity * 2 * Z);
            const uint64_t new_capacity = temp_buffer.size() / Z;

            // Align old indices based on the new capacity
            // Below code tries to reduce the number of additions

            // Move first row
            for (uint64_t col = 0; col < column_count; ++col)
                temp_buffer[col] = std::move(buffer[col]);

            // Initialize the index as the first shifted index
            uint64_t old_index = available_capacity;
            uint64_t new_index = new_capacity;

            // Move second row
            for (uint64_t col = 0; col < column_count; ++col)
                temp_buffer[new_index + col] = std::move(buffer[old_index + col]);

            // Move remaining rows
            for (uint64_t row = 2; row < Z; ++row)
            {
                old_index += available_capacity;
                new_index += new_capacity;

                for (uint64_t col = 0; col < column_count; ++col)
                    temp_buffer[new_index + col] = std::move(buffer[old_index + col]);
            }

            buffer = std::move(temp_buffer);
        }

        column_count += p_num;
    }

    void set_cell(uint64_t p_column, uint64_t p_row, const godot::Variant& p_value)
    {
        ERR_FAIL_COND(column_count <= p_column);
        ERR_FAIL_COND(Z <= p_row);

        const uint64_t index = p_column + p_row * row_capacity();
        ERR_FAIL_COND(buffer.size() <= index);

        buffer[index] = p_value;
    }

    const godot::Variant& get_cell(uint64_t p_column, uint64_t p_row) const
    {
        static const godot::Variant nil;
        ERR_FAIL_COND_V(column_count <= p_column, nil);
        ERR_FAIL_COND_V(Z <= p_row, nil);

        const uint64_t index = p_column + p_row * row_capacity();
        ERR_FAIL_COND_V(buffer.size() <= index, nil);

        return buffer[index];
    }

private:
    uint64_t column_count = 0;
    std::vector<godot::Variant> buffer;
};

} // namespace morphy

#endif // MORPHY_ROWS_CLASS_H
