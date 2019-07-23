s = 1/8--1/8
wall_r = 0
r = {wall_r,wall_r, wall_r}
-- mat1 = gr.perlin_material(64, {s,s,s}, {0,1,0}, {0.5,0.5,0.5}, 25, 0, 0, 0, 0, 0)
-- mat4 = gr.cmaterial({1, 1, 1}, {0.1, 0.1, 0.1}, 25, 0.8, 1.1, 0, 0)
plain_mat = gr.material({0.9,0.9,0.9}, {0.15,0.15,0.15}, 100)

root = gr.node('root')

-- Setup Cornell box
dofile("cornell_box.lua")

-- Toss things into the room!
s6 = gr.mesh( 'poly', '../objs/grid_cube/grid_cube.obj' )
s6:rotate('y', 25)
-- s6:rotate('x', -15)
s6:scale(35,35,35)
s6:translate(40, 35, -10)
root:add_child(s6)
s6:set_material(plain_mat)

-- Lighting
point_light = true;
point_light_brightness = 1

if point_light then

else
	point_light_brightness = 0

	white_area_light = gr.area_light("spotlight ugh", {0.9,0.9,0.9}, {1,0,0})
	ls = 50
	white_area_light:scale(ls,ls,ls)
	white_area_light:rotate('X', -90)
	white_area_light:translate(-25,199,75)

	root:add_child(white_area_light)
end

white_light = gr.light({0, 190, 50}, {point_light_brightness,point_light_brightness,point_light_brightness}, {1.2,0,0})--{0,0,0.00008})

size = 1024

gr.render(root, 'images/output.png', size, size,-- 512, 512, --256, 256, --1024, 1024,
	  {0, 100, 350}, {0, 100, 0}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})