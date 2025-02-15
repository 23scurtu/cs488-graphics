-- A simple scene with five spheres

-- mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 0)
s = 2
mat1 = gr.perlin_material(64, {s,s,s}, {0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 0, 0, 0, 0, 0)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25, 0)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.5, 0.7, 0.5}, 25, 0)
mat4 = gr.cmaterial({1, 1, 1}, {0.1, 0.1, 0.1}, 25, 0, 1.2, 0, 0)
mat5 = gr.cmaterial({1, 1, 1}, {0.1, 0.1, 0.1}, 25, 0.8, 1.2, 0, 0)

scene_root = gr.node('root')

s1 =  gr.cmesh( 'poly', '../objs/teapot/teapot.obj', true) --gr.nh_sphere('s1', {0, 0, -400}, 100)
s1:scale(20,20,20)
s1:rotate('y', -30)
s1:translate(0,0,0) --760
scene_root:add_child(s1)
s1:set_material(mat1)

-- s2 = gr.nh_sphere('s2', {200, 50, -100}, 150)
-- scene_root:add_child(s2)
-- s2:set_material(mat1)

-- s3 = gr.nh_sphere('s3', {0, -1200, -500}, 1000)
-- scene_root:add_child(s3)
-- s3:set_material(mat2)

-- s4 = gr.nh_sphere('s4', {-100, 25, -300}, 50)
-- scene_root:add_child(s4)
-- s4:set_material(mat3)

-- s5 = gr.nh_sphere('s5', {0, 100, -250}, 25)
-- scene_root:add_child(s5)
-- s5:set_material(mat1)

-- s6 = gr.nh_sphere('s6', {0, 0, 700}, 25)
-- scene_root:add_child(s6)
-- s6:set_material(mat4)

white_light = gr.light({-100.0, 100, 1200.0}, {0.9, 0.9, 0.9}, {1, 0, 0})
-- magenta_light = gr.light({400.0, 100.0, 150.0}, {0.7, 0.0, 0.7}, {1, 0, 0})

size = 512

gr.render(scene_root, 'simple.png', size, size,-- 512, 512, --256, 256, --1024, 1024,
	  {0, 0, 800}, {0, 0, -800}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {white_light, magenta_light})
