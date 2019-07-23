SHOW_EFFECT = true

s = 1/8--1/8
wall_r = 0
r = {wall_r,wall_r, wall_r}
mat1 = gr.cmaterial({1,1,0.3}, {0.1,0.1,0.1}, 25, 0, 0, 0, 0, 0)
-- mat4 = gr.cmaterial({1, 1, 1}, {0.1, 0.1, 0.1}, 25, 0.8, 1.1, 0, 0)
mat2 = gr.material({0.5,0.25,0}, {0.1,0.1,0.1}, 25)

root = gr.node('root')

-- s1 = gr.sphere('s1', {0, 0, 0})
-- s1:scale(50,50,50)
-- s1:translate(-35,50, 25)
-- root:add_child(s1)
-- if SHOW_EFFECT then
-- s1:set_material(mat1)
-- else
-- s1:set_material(mat2)
-- end

s2 = gr.mesh( 'poly', '../objs/grid_cube/grid_cube_bumpy.obj')
s2:scale(35,35,35)
s2:rotate('X', 0)
s2:rotate('Y', 25)
s2:translate(0,35, 0)
root:add_child(s2)
s2:set_material(mat2)

s3 = gr.mesh( 'poly', '../objs/grid_cube/grid_cube_bumpy.obj')
s3:scale(35,35,35)
s3:rotate('X', 0)
s3:rotate('Y', -15)
s3:translate(0,105, 0)
root:add_child(s3)
s3:set_material(mat2)

-- Setup Cornell box
dofile("cornell_box.lua")

-- Toss things into the room!

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

if SHOW_EFFECT then
light_pos = {-50, 190, 100}
else
light_pos = {50, 190, 100}
end

white_light = gr.light(light_pos, {point_light_brightness,point_light_brightness,point_light_brightness}, {1.2,0,0})--{0,0,0.00008})

size = 512

gr.render(root, 'images/output.png', size, size,-- 512, 512, --256, 256, --1024, 1024,
	  {0, 100, 350}, {0, 100, 0}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})