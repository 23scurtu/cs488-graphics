-- SHOW_EFFECT = true

s = 1/8--1/8
wall_r = 0.1
r = {wall_r,wall_r, wall_r}
mat1 = gr.cmaterial({1,1,0.3}, {0.1,0.1,0.1}, 25, 0, 0, 0, 0, 0)
-- mat4 = gr.cmaterial({1, 1, 1}, {0.1, 0.1, 0.1}, 25, 0.8, 1.1, 0, 0)
mat2 = gr.material({0.5,0.25,0}, {0.1,0.1,0.1}, 25)
red_mat = gr.material({1,0.15,0.15}, r, 25, 0)

clear_transparent_13_mat = gr.cmaterial({0,0,0}, {0.1,0.1,0.1}, 100, 1, 1.3, 0, 0)

clear_transparent_1_mat = gr.cmaterial({0.9,0.9,0.9}, {1,1,1}, 100, 1, 1.3, 0, 0)

mirror = gr.material({0.2,0.2,0.2}, {0.75,0.75,0.75}, 100)
double_sided_mirror = gr.cmaterial({0.9,0.9,0.9}, {0.9,0.9,0.9}, 100, 1, 1, 0, 0)

white_mat = gr.material({1, 0.95, 0.8}, r, 25, 0)

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

n1 = gr.node('glass_orb')
n1:scale(25,25,25)
n1:translate(-50,25, 0)
root:add_child(n1)

s1 = gr.sphere('s1', {0, 0, 0})
n1:add_child(s1)
-- if SHOW_EFFECT then
-- 	s1:set_material(blue_transparent_mat)
-- else
s1:set_material(clear_transparent_13_mat)

-- Infinity mirror
inf_mirror = gr.node('infinity_mirror')
inf_mirror:scale(50,50,25)
inf_mirror:translate(-75,25, -200)
root:add_child(inf_mirror)

s4 = gr.cube('s1', {0, 0, 0})
s4:scale(25,25,1)
-- s4:rotate('X', -90)
-- s4:rotate('Y', -25)
s4:translate(-12.5,-5, 25)
s4:set_material(mirror)
inf_mirror:add_child(s4)

s5 = gr.cube('s1', {0, 0, 0})
s5:scale(3,3,1)
-- s4:rotate('X', -90)
s5:rotate('Y', 0)
s5:translate(0,0, 0)
s5:set_material(mirror)
inf_mirror:add_child(s5)

s6 = gr.sphere('ball', {0,0,0})
s6:scale(25,25,25)
s6:translate(50,200,0)
s6:set_material(red_mat)
root:add_child(s6)


-- s2 = gr.sphere('s1', {0, 0, 0})
-- s2:scale(0.6,0.6,0.6)
-- n1:add_child(s2)
-- -- if SHOW_EFFECT then
-- -- 	s1:set_material(blue_transparent_mat)
-- -- else
-- s2:set_material(clear_transparent_13_mat)

plane = gr.mesh( 'back_wall', '../objs/brick_plane/brick_plane.obj')  --'../plane.obj' )
root:add_child(plane)
ps = 275
plane:scale(ps, ps, ps)
-- plane:rotate('Y', 90)
plane:rotate('X', 90)
-- plane:rotate('Y', 20)
plane:translate(0, 90, -200)
plane:set_material(white_mat)



s2 = gr.mesh( 'poly', '../objs/wood_plane/wood_plane.obj')
ps = 275
s2:scale(ps,ps,ps)
s2:rotate('X', 0)
-- s2:rotate('Y', 25)
s2:translate(0,0, 0)
root:add_child(s2)
s2:set_material(mat2)

-- Setup Cornell box
-- dofile("cornell_box.lua")

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


light_pos = {-100, 500, 100}


white_light = gr.light(light_pos, {point_light_brightness,point_light_brightness,point_light_brightness}, {1.2,0,0})--{0,0,0.00008})

size = 4096

gr.render_options(20,true, 2, 3,2,2, {4,2}, 250)

gr.render(root, 'images/output.png', size, size,-- 512, 512, --256, 256, --1024, 1024,
	  {0, 100, 350}, {0, 100, 0}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})