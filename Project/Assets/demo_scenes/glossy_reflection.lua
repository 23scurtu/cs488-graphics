SHOW_EFFECT = true

s = 1/8--1/8
wall_r = 0
r = {wall_r,wall_r, wall_r}
-- mat1 = gr.perlin_material(64, {s,s,s}, {0,1,0}, {0.1,0.1,0.1}, 25, 0, 0, 0, 0, 0)
-- mat4 = gr.cmaterial({1, 1, 1}, {0.1, 0.1, 0.1}, 25, 0.8, 1.1, 0, 0)

grey_shiny_mat = gr.material({0.2,0.2,0.2}, {0.6,0.6,0.6}, 100)
grey_glossy_mat = gr.cmaterial({0.2,0.2,0.2}, {0.6, 0.6, 0.6}, 100, 0, 0, 0.3, 0)

root = gr.node('root')

s1 = gr.sphere('s1', {0, 0, 0})
s1:scale(50,50,50)
s1:translate(-25,50, -25)
root:add_child(s1)
if SHOW_EFFECT then
	s1:set_material(grey_glossy_mat)
else
	s1:set_material(grey_shiny_mat)
end

s2 = gr.sphere('s2', {0, 0, 0})
s2:scale(25,25,25)
s2:translate(50,25, 25)
root:add_child(s2)
if SHOW_EFFECT then
	s2:set_material(grey_glossy_mat)
else
	s2:set_material(grey_shiny_mat)
end

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

white_light = gr.light({0, 190, 50}, {point_light_brightness,point_light_brightness,point_light_brightness}, {1.2,0,0})--{0,0,0.00008})

size = 512

gr.render(root, 'images/output.png', size, size,-- 512, 512, --256, 256, --1024, 1024,
	  {0, 100, 350}, {0, 100, 0}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})