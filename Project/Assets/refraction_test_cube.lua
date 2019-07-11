-- A simple scene with five spheres

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 0)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25, 0)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25, 0)
mat4 = gr.cmaterial({1, 1, 1}, {0.1, 0.1, 0.1}, 25, 0.8, 1.1)


scene_root = gr.node('root')

s1 = gr.nh_sphere('s1', {0, 0, -400}, 100)
scene_root:add_child(s1)
s1:set_material(mat1)

s2 = gr.nh_box('s2', {200, 50, -100}, 150)
scene_root:add_child(s2)
s2:set_material(mat1)

plane = gr.mesh( 'plane', 'plane.obj' )
scene_root:add_child(plane)
ps = 3200
plane:scale(ps, ps, ps)
-- plane:rotate('Y', 90)
-- plane:rotate('X', 90)
-- plane:rotate('Y', 20)
plane:translate(-8, -400, -1600)
plane:set_material(mat1)

-- s6 = gr.cube('s6')
-- -- s6:
-- s6:rotate('y', 30)
-- s6:scale(25,25,25)
-- s6:translate(0, 0, 700)
-- scene_root:add_child(s6)
-- s6:set_material(mat4)

s6 = gr.mesh( 'poly', 'dodeca.obj' )
s6:rotate('y', 30)
s6:scale(25,25,25)
s6:translate(0, 0, 500)
scene_root:add_child(s6)
s6:set_material(mat4)

white_light = gr.light({-100.0, 150.0, 400.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
magenta_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

size = 512

gr.render(scene_root, 'refraction_test_cube.png', size, size,-- 512, 512, --256, 256, --1024, 1024,
	  {0, 0, 800}, {0, 0, -800}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, magenta_light})
