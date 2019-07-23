SHOW_EFFECT = true

s = 1/8--1/8
wall_r = 0
r = {wall_r,wall_r, wall_r}
mat1 = gr.perlin_material(64, {s,s,s}, {0,1,0}, {0.1,0.1,0.1}, 25, 0, 0, 0, 0, 0)
-- mat4 = gr.cmaterial({1, 1, 1}, {0.1, 0.1, 0.1}, 25, 0.8, 1.1, 0, 0)
mat2 = gr.material({1,1,1}, {0.1,0.1,0.1}, 25)

root = gr.node('root')

s1 = gr.sphere('s1', {0, 0, 0})
s1:scale(50,50,50)
s1:translate(-35,50, 25)
root:add_child(s1)
s1:set_material(mat1)


s2 = gr.cube('s1', {0, 0, 0})
s2:scale(50,50,50)
s2:rotate('X', -90)
s2:rotate('Y', 25)
s2:translate(50,0, 125)
root:add_child(s2)
s2:set_material(mat1)

-- Setup Cornell box
dofile("cornell_box.lua")

-- Toss things into the room!

-- Lighting
point_light = not SHOW_EFFECT;
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

size = 512

gr.render(root, 'images/output.png', size, size,-- 512, 512, --256, 256, --1024, 1024,
	  {0, 100, 350}, {0, 100, 0}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})