extends Node

const column_increment := 5
const row_increment := 5

@onready var debug_text := $debug_text as RichTextLabel

func _ready():
	for i in 5:
		test_tables()
	test_expression_view()
	test_expression_view_stress()
	test_shader_view()
	

func test_tables():
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

func test_expression_view():
	var table = BasicTable.new()
	table.add_columns(PackedStringArray(["a", "b"]))
	table.add_rows(1)

	table.set_cell(0, 0, 1)
	table.set_cell(1, 0, 2)

	var start := Time.get_ticks_usec()

	var c := ExpressionColumn.new()
	c.set_name("c")
	c.set_expression("column('e').cell(row())")
	
	var d := ExpressionColumn.new()
	d.set_name("d")
	d.set_expression("column('c').cell(row()) + column('e').cell(row())")
	
	var e := ExpressionColumn.new()
	e.set_name("e")
	e .set_expression("column('a').cell(row()) + column('b').cell(row())")

	var view := ExpressionView.new()
	view.set_view(table)
	view.add_expressions([c, d, e])

	print("Expressions took ", (Time.get_ticks_usec() - start) / 1000.0 , " milliseconds.")

	assert(view.get_cell(0, 0) == 1)
	assert(view.get_cell(1, 0) == 2)
	assert(view.get_cell(2, 0) == 3)
	assert(view.get_cell(3, 0) == 6)
	assert(view.get_cell(4, 0) == 3)
	assert(view.num_columns() == 5)
	assert(view.num_rows() == 1)

func test_expression_view_stress():
	var table = BasicTable.new()
	table.add_columns(PackedStringArray(["a", "b"]))
	var num_rows := 100000
	table.add_rows(num_rows)

	for row in num_rows:
		table.set_cell(0, row, row)
		table.set_cell(1, row, row + 1)

	var c := ExpressionColumn.new()
	c.set_name("c")
	c.set_expression("column('a').cell(row()) + column('b').cell(row())")

	var d := ExpressionColumn.new()
	d.set_name("d")
	d.set_expression("column('a').cell(row()) + column('b').cell(row()) + column('c').cell(row())")

	var start := Time.get_ticks_usec()

	var view := ExpressionView.new()
	view.set_view(table)
	view.add_expressions([c, d])

	debug_text.text += "Expressions took " + String.num((Time.get_ticks_usec() - start) / 1000.0) + " milliseconds.\n"

	for row in num_rows:
		var value = view.get_cell(2, row)
		assert(value == (row + row + 1))


func test_shader_view():
	var table = BasicTable.new()
	table.add_columns(PackedStringArray(["a", "b"]))
	var num_rows := 10000
	table.add_rows(num_rows)
	for row in num_rows:
		table.set_cell(0, row, row)
		table.set_cell(1, row, row + 1)

	var column = ShaderColumn.new()
	column.set_name("shader")
	column.set_shader("""
		float compute_cell(row_t row) {
			return row.a + row.b;
		}
	""", ["a", "b"])

	var start := Time.get_ticks_usec()

	var view := ShaderView.new()
	view.set_view(table)
	view.add_columns([column])

	debug_text.text += "Shaders took " + String.num((Time.get_ticks_usec() - start) / 1000.0) + " milliseconds.\n"

	for row in num_rows:
		var value = view.get_cell(2, row)
		assert(value == (row + row + 1))
