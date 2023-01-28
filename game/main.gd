extends Node

const column_increment := 5
const row_increment := 25


func _ready() -> void:
	test_add()
	return

	var start := Time.get_ticks_usec()
	var table := BasicTable.new()

	for i in 25:
		assert(i * column_increment == table.num_columns())
		assert(i * row_increment == table.num_rows())

		for row in table.num_rows():
			for column in table.num_columns():
				table.set_cell(column, row, row * table.num_columns() + column)

		for row in table.num_rows():
			for column in table.num_columns():
				assert(table.get_cell(column, row) == row * table.num_columns() + column)

		var columns = PackedStringArray()
		for c in column_increment:
			columns.push_back("{0}".format([table.num_columns() + c]))
		table.add_columns(columns)

		table.add_rows(row_increment)

		for row in table.num_rows() - row_increment:
			for column in table.num_columns() - column_increment:
				assert(table.get_cell(column, row) == row * (table.num_columns() - column_increment) + column)

	print("It took ", (Time.get_ticks_usec() - start) / 1000.0 , " milliseconds.")


func test_add():
	var table := BasicTable.new()
	table.add_columns(PackedStringArray(["a", "b"]))
	table.add_rows(1)
	print(table.num_columns())
	print(table.num_rows())

	table.set_cell(0, 0, 1)
	table.set_cell(1, 0, 2)

	var add := ExpressionColumn.new()
	add.set_name("add")
	add.set_expression("a[row()] + b[row()]")

	var view := ExpressionView.new()
	#view.set_view(table)
	view.add_expressions([add])

	return
	assert(view.get_cell(0, 0) == 1)
	assert(view.get_cell(1, 0) == 2)
	assert(view.get_cell(2, 0) == 3)
