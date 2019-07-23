-- A simple scene with five spheres

mat1 = gr.material({0.7, 1.0, 0.7}, {0.5, 0.7, 0.5}, 25, 0)
mat2 = gr.material({0.5, 0.5, 0.5}, {0.5, 0.7, 0.5}, 25, 0)
mat3 = gr.material({1.0, 0.6, 0.1}, {0.3, 0.3, 0.3}, 25, 0)
mat4 = gr.cmaterial({1, 1, 1}, {0.1, 0.1, 0.1}, 25, 0.8, 1.1, 0, 0)

mat5 = gr.material({1.0, 0.6, 0.1}, {0,0,0}, 25, 0)


scene_root = gr.node('root')
scene_root:translate(0,0,0);

math.randomseed(1234)
min = -30
max = 30

for i=1,100 do

	gg = gr.nh_sphere('gg', {math.random(min, max), math.random(min, max), math.random(min, max)}, 1)

	gg:translate(0,0,0)
	gg:set_material(mat1)
	scene_root:add_child(gg)
end

black_light = gr.light({20, 100, 0}, {1,1,1}, {2, 0, 0})

size = 1024

-- gr.render_options(5,true, 4, 3,2,2, {4,2}, 100)

gr.render(scene_root, 'images/output.png', size, size,-- 512, 512, --256, 256, --1024, 1024,
	  {5,5,80}, {0, 0, 0}, {0, 1, 0}, 50,
	  {0.3, 0.3, 0.3}, {black_light})
