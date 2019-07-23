-- A simple scene with five spheres

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 0)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25, 0)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.3, 0.3, 0.3}, 25, 0)
mat4 = gr.cmaterial({1, 1, 1}, {0.1, 0.1, 0.1}, 25, 0.8, 1.1, 0, 0)

mat5 = gr.material({1.0, 0.6, 0.1}, {0,0,0}, 25, 0)


scene_root = gr.node('root')

s1 = gr.nh_sphere('s1', {0, 0, 0}, 100)
scene_root:add_child(s1)
s1:set_material(mat1)

-- s2 = gr.nh_box('s2', {200, 50, -100}, 150)
-- scene_root:add_child(s2)
-- s2:set_material(mat1)

plane = gr.mesh( 'plane', 'plane.obj' )
scene_root:add_child(plane)
ps = 800
plane:scale(ps, ps, ps)
-- plane:rotate('Y', 90)
-- plane:rotate('X', 90)
-- plane:rotate('Y', 20)
plane:translate(-8, -200, 0)
plane:set_material(mat1)

-- s6 = gr.cube('s6')
-- -- s6:
-- s6:rotate('y', 30)
-- s6:scale(25,25,25)
-- s6:translate(0, 0, 700)
-- scene_root:add_child(s6)
-- s6:set_material(mat4)

-- s6 = gr.mesh( 'poly', '../objs/grid_cube/grid_cube_bumpy.obj' )
-- s6:rotate('y', 45)
-- s6:rotate('x', -15)
-- s6:scale(25,25,25)
-- s6:translate(0, 30, 700)
-- scene_root:add_child(s6)
-- s6:set_material(mat5)

-- white_light = gr.light({200, 200, 0}, {0.9, 0.9, 0.9}, {1, 0, 0})
-- white_light2 = gr.light({0,0, 2000.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
-- magenta_light = gr.light({0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

black_light = gr.light({200, 200, 0}, {0,0,0}, {1, 0, 0})

white_area_light = gr.area_light("spotlight ugh", {0.9,0.9,0.9}, {1,0,0})
ls = 100
white_area_light:rotate('x',-30)
white_area_light:rotate('y',90)

white_area_light:scale(ls,ls,ls)
white_area_light:translate(400,200, 100)

scene_root:add_child(white_area_light)



size = 512

gr.render(scene_root, 'simple.png', size, size,-- 512, 512, --256, 256, --1024, 1024,
	  {0, 500, 2000}, {0, 0, 0}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {black_light})
