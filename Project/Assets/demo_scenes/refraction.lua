SHOW_EFFECT = true

s = 1/8--1/8
wall_r = 0
r = {wall_r,wall_r, wall_r}
-- mat1 = gr.perlin_material(64, {s,s,s}, {0,1,0}, {0.1,0.1,0.1}, 25, 0, 0, 0, 0, 0)
-- mat4 = gr.cmaterial({1, 1, 1}, {0.1, 0.1, 0.1}, 25, 0.8, 1.1, 0, 0)

blue_transparent_mat = gr.cmaterial({0.3,0.3,0.7}, {0.7,0.7,0.3}, 100, 1, 1.4, 0, 0)
clear_transparent_08_mat = gr.cmaterial({0.9,0.9,0.9}, {0.1,0.1,0.1}, 100, 1, 0.9, 0, 0)
clear_transparent_13_mat = gr.cmaterial({0.9,0.9,0.9}, {0.1,0.1,0.1}, 100, 1, 1.3, 0, 0)
green_transparent_15_mat = gr.cmaterial({0.1,0.9,0.1}, {0.9,0.1,0.9}, 100, 1, 1.5, 0, 0)
clear_transparent_1_mat = gr.cmaterial({0.9,0.9,0.9}, {0.1,0.1,0.1}, 100, 1, 1.0, 0, 0)
-- mat4 = gr.cmaterial({1, 1, 1}, {0.1, 0.1, 0.1}, 25, 0.8, 1.2, 0, 0.00003)

green_mat = gr.material({0.1,0.9,0.1}, {0,0,0}, 100)

blue_mat = gr.material({0,0,0.7}, {0,0,0}, 100)
plain_mat = gr.material({0.9,0.9,0.9}, {0,0,0}, 100)


root = gr.node('root')

s1 = gr.sphere('s1', {0, 0, 0})
s1:scale(50,50,50)
s1:translate(-50,50, 0)
root:add_child(s1)
-- if SHOW_EFFECT then
-- 	s1:set_material(blue_transparent_mat)
-- else
s1:set_material(blue_mat)
-- end

-- s4 = gr.sphere('s4', {0, 0, 0})
-- s4:scale(25,25,25)
-- s4:translate(-25,100, 50)
-- root:add_child(s4)
-- if SHOW_EFFECT then
-- 	s4:set_material(blue_transparent_mat)
-- else
-- 	s4:set_material(blue_mat)
-- end

-- s2 = gr.sphere('s2', {0, 0, 0})
-- s2:scale(25,25,25)
-- s2:translate(65,25, -25)
-- root:add_child(s2)
-- if SHOW_EFFECT then
-- 	s2:set_material(clear_transparent_mat)
-- else
-- 	s2:set_material(plain_mat)
-- end

s = 10

s3 = gr.sphere('s2', {0, 0, 0})
s3:scale(s,s,s)
s3:translate(-25,100, 250)
root:add_child(s3)

if SHOW_EFFECT then
	s3:set_material(clear_transparent_08_mat)
else
	s3:set_material(plain_mat)
end

s6 = gr.sphere('s2', {0, 0, 0})
s6:scale(s,s,s)
s6:translate(0,75, 250)
root:add_child(s6)

if SHOW_EFFECT then
	s6:set_material(green_transparent_15_mat)
else
	s6:set_material(green_mat)
end

s4 = gr.sphere('s2', {0, 0, 0})
s4:scale(s,s,s)
s4:translate(0,100, 250)
root:add_child(s4)

if SHOW_EFFECT then
	s4:set_material(clear_transparent_13_mat)
else
	s4:set_material(plain_mat)
end

s5 = gr.sphere('s2', {0, 0, 0})
s5:scale(s,s,s)
s5:translate(25,100, 250)
root:add_child(s5)

if SHOW_EFFECT then
	s5:set_material(clear_transparent_1_mat)
else
	s5:set_material(plain_mat)
end


-- blue_transparent_mat = gr.cmaterial({0.3,0.3,0.7}, {0.7,0.7,0.3}, 100, 1, 1.4, 0, 0)
-- clear_transparent_mat = gr.cmaterial({0.9,0.9,0.9}, {0.1,0.1,0.1}, 100, 1, 1.3, 0, 0)
-- -- mat4 = gr.cmaterial({1, 1, 1}, {0.1, 0.1, 0.1}, 25, 0.8, 1.2, 0, 0.00003)

-- blue_mat = gr.material({0,0,0.7}, {0,0,0}, 100)
-- plain_mat = gr.material({0.9,0.9,0.9}, {0,0,0}, 100)


-- root = gr.node('root')

-- s1 = gr.sphere('s1', {0, 0, 0})
-- s1:scale(50,50,50)
-- s1:translate(-50,50, 0)
-- root:add_child(s1)
-- -- if SHOW_EFFECT then
-- -- 	s1:set_material(blue_transparent_mat)
-- -- else
-- 	s1:set_material(blue_mat)
-- -- end

-- -- s4 = gr.sphere('s4', {0, 0, 0})
-- -- s4:scale(25,25,25)
-- -- s4:translate(-25,100, 50)
-- -- root:add_child(s4)
-- -- if SHOW_EFFECT then
-- -- 	s4:set_material(blue_transparent_mat)
-- -- else
-- -- 	s4:set_material(blue_mat)
-- -- end

-- s2 = gr.sphere('s2', {0, 0, 0})
-- s2:scale(25,25,25)
-- s2:translate(65,25, -25)
-- root:add_child(s2)
-- if SHOW_EFFECT then
-- 	s2:set_material(clear_transparent_mat)
-- else
-- 	s2:set_material(plain_mat)
-- end



-- if SHOW_EFFECT then
	
-- 	s3 = gr.sphere('s2', {0, 0, 0})
-- 	s3:scale(25,25,25)
-- 	s3:translate(0,100, 250)
-- 	s3:set_material(clear_transparent_mat)
-- 	root:add_child(s3)
-- -- else
-- -- 	s3:set_material(plain_mat)
-- end

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
-- white_light2 = gr.light({-90, 100, 400}, {point_light_brightness,point_light_brightness,point_light_brightness}, {1.2,0,0})
size = 1024

-- gr.render_options(100,false, 2, 3,2,2, {4,2}, 100)

gr.render(root, 'images/output.png', size, size,-- 512, 512, --256, 256, --1024, 1024,
	  {0, 100, 350}, {0, 100, 0}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light})