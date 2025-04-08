# Introduction

A 3D software renderer implemented from "scratch", on Windows. More specifically everything that is directly related to rendering other than SDL.

**This started as a proof of concept, without me copying standard implementations/algorithms/functions, and intuition was prioritized over performance. As a byproduct of this, the renderer only works smoothly with very low polygon counts, which is also why `Flat` shading is the default option for [light](#light) shading over `Gouraud` and `Phong` shading, otherwise you often would be unable to do anything due to extremely low FPS. You can change the shading type in the [light tab](#light-tab). You can also try making the window smaller by resizing it.**
 
**Make sure to update the [scene](#scene) folder and models folder in the menu to where you downloaded or intend to store [scenes](#scene) and models. You can also change the default values in `SceneTab.h` and recompile.**

**A transform axis is, by default (can be disabled in the menu), drawn over the selected [instance](#instance)/[light source](#light). The [mesh](#mesh) for that transform axes is called `axes.obj`. The one I used can be found in this repo's `models` folder. This [mesh](#mesh) NEEDS to be in your models folder. You could use another [mesh](#mesh) to represent the transform axes, but in that case you would need to rename that [mesh](#mesh) to "axes.obj" and place it within your models folder.**

Depth testing and shading is purposefully disabled for the transform axes, so that it can be seen in front of other objects and with a distinct, consistent color.

<h3>Libraries used</h3>

| Library      | Version | Usage | URL     |
| :---       |  :----:       |   :----:   |          ---: |
| SDL2        | 2.28.1 | Rendering | https://github.com/libsdl-org/SDL/tree/release-2.28.1 |
| Catch2      |  3.4.0  |  Testing       | https://github.com/catchorg/Catch2/releases/tag/v3.4.0   |
| Nlohmann JSON   |   3.11.2   |   JSON parsing        | https://github.com/nlohmann/json/releases/tag/v3.11.2      |
| ImGUI      |  1.91.9  |  Menus/user interface       | https://github.com/ocornut/imgui/releases/tag/v1.91.9   |
| Nativefiledialog   |   116   |   Windows file dialogs        | https://github.com/mlabbe/nativefiledialog/releases/tag/release_116      |

# Examples
> The videos are sped up and the quality is not the best due to GitHub's 10MB file limit.

## Lighting

https://github.com/user-attachments/assets/1a2aa3cd-cffe-4fa4-a9d4-34ec3d63f956

## Camera

https://github.com/user-attachments/assets/1cf5010c-1034-43ce-8cc9-aba36a9f05d8

## Scene editing

https://github.com/user-attachments/assets/57838ede-8113-4c9c-af6e-c8be609580ee

## Scene loading

https://github.com/user-attachments/assets/5a662674-1898-435d-b98c-8c73a31e21eb

## Creating a scene within the renderer


**<h2>Part 1/2</h2>**

https://github.com/user-attachments/assets/04061dc5-9c95-4505-b231-3657f18fe05b


<h2>**Part 2/2**</h2>

https://github.com/user-attachments/assets/140846a2-51cc-425f-bf6f-501e04a37186


## Creating a scene from a json file


https://github.com/user-attachments/assets/7de72ded-9071-476d-9c69-95ff55d56355


## Updating scene and model folders

https://github.com/user-attachments/assets/52bd4d7a-54f3-4973-93e8-652a013050ef


## Creating a blank scene

https://github.com/user-attachments/assets/b991f7ed-b429-4ce4-8438-995679e479a0


> On the showcases which [instances](#instance) are added to the [scene](#scene), sometimes the names in the [instance](#instance) list were not correct, but this has been fixed. More specifically, the name on the list should be the name you gave it in the "instance name" text box, whereas if it was empty, its name should become `mesh_id`, where "mesh" is the name of its mesh, and `id` is the [instance's](#instance) id. This has been corrected, and it is the case now.


# Table of contents
- [Introduction](#introduction)
- [Examples](#examples)
  - [Lighting](#lighting)
  - [Camera](#camera)
  - [Scene editing](#scene-editing)
  - [Scene loading](#scene-loading)
  - [Creating a scene within the renderer](#creating-a-scene-within-the-renderer)
  - [Creating a scene from a json file](#creating-a-scene-from-a-json-file)
  - [Updating scene and model folders](#updating-scene-and-model-folders)
  - [Creating a blank scene](#creating-a-blank-scene) 
- [Matrix](#matrix)
- [Engine](#engine)
- [Triangle](#triangle)
- [Quad](#quad)
- [Mesh](#mesh)
- [Instance](#instance)
- [Scene](#scene)
- [Camera](#camera)
- [Light](#light)
- [Rotation](#rotation)
  - [Quaternion](#quaternion)
- [Windows and tabs (menus)](#windows-and-tabs-menus)
  - [Window manager](#window-manager)
  - [Window](#window)
  - [General window](#general-window)
    - [Settings tab](#settings-tab)
    - [Scene tab](#scene-tab)
      - [Camera tab](#camera-tab)
      - [Instances tab](#instances-tab)
      - [Light tab](#light-tab)
- [Events](#events)
- [main.cpp](#maincpp)
- [Utils](#utils)
- [Implementation details](#implementation-details)
- [Testing](#testing)
- [Notes](#notes)
- [Todo](#todo)

# Matrix

The `Matrix.h` and `Matrix.cpp` files are the header and implementation files for the [`Mat`](#matrix) class which represents a matrix.

Internally, the representation of a [matrix](#matrix) is that of a 2-dimensional array (`double**`).

The [matrix](#matrix) multiplication order is left-to-right. This means that the [matrix](#matrix)/vector to be transformed must be to the right of the transformation matrices, and the number of columns in the left [`Mat`](#matrix) must match the number of rows in the right [`Mat`](#matrix).
(i.e. if multiplying two vectors, the left vector must be a row vector (`1xN`), while the right one must be a column vector (`Nx1`))

The order of operations is: Scaling -> rotation -> translation.
i.e. `world_space_vector = TRANSLATION_MATRIX * ROTATION_MATRIX * SCALING_MATRIX * local_space_vector`
> This order is reversed for the view [matrix](#matrix) since it represents the opposite transformation of the vertices

You can create a [`Mat`](#matrix) instance by calling the initializer function with a 2-dimensional `std::initializer_list`, which is essentially a 2-dimensional array which will hold the rows and columns for the [matrix](#matrix), followed by the row dimension and column dimension.
>Example for a `2x3` [matrix](#matrix): `Mat({{1, 2, 3}, {3, 4, 5}}, 2, 3)`

You can get a value from the [matrix](#matrix) by calling the `get` [`Mat`](#matrix) member function.
> Example to get the value in the 2nd row and 3rd column: `matrix.get(2, 3)`

You can set a value in the [matrix](#matrix) by calling the `set` [`Mat`](#matrix) member function.
> Example to set the value in the 2nd row and 3rd column to 5: `matrix.set(5, 2, 3)`

You can transpose a [matrix](#matrix) by calling the `transposed` [`Mat`](#matrix) member function.
> Example: `matrix.transposed()`

You can (assuming that the dimensions are valid) add `+`, add and assign `+=`, subtract `-`, subtract and assign `-=`, multiply by  `*`, and multiply and assign `*=`.

You can also perform element-wise operations by combining a `scalar` with one of the following operators and a [`Mat`](#matrix), by multiplying `*`, multiplying and assigning `*=`, dividing `/`, and dividing and assigning `/=`.

You can calculate the dot product between vectors by calling the `static` [`Mat`](#matrix) function `dot`. This operation is commutative (i.e. order does not matter, neither does whether the vectors are row or column). The only requirements are that both [`Mat`](#matrix) must be vectors with the same number of elements (i.e. either the row dimension or column dimension must be `1` while the other dimensions match).
> Example: `Mat::dot(vector_a, vector_b)`

You can calculate the vector norm (magnitude/length) by calling the [`Mat`](#matrix) member function `norm`.
> Example: `vec_a.norm()`

You can normalize a vector by calling the `normalize` [`Mat`](#matrix) member function. This operation divides the [matrix](#matrix), element-wise, by its norm/magnitude/length. This turns it into a unit vector (norm/magnitude/length of `1`).
> Example: `matrix.normalize()`

You can test for equality and inequality, reassign a [`Mat`](#matrix) instance to another [`Mat`](#matrix) instance through copy & reassignment, and you can also directly reassign it to a new [`Mat`](#matrix) instance.

You can print the [matrix](#matrix) to the console by calling the `print` [`Mat`](#matrix) member function.
> Example: `matrix.print()`<br>
> You can also print it to the console using streams. i.e. by calling `std::cout << [matrix](#matrix)`.

You can get an identity [matrix](#matrix) of an arbitrary dimension by calling the `identity_matrix` [`Mat`](#matrix) static function, which returns a square [matrix](#matrix) of `NxN` dimension, where `N` is the given dimension.
> Example: `Matrix::identity_matrix(4)`

You can get a translation [matrix](#matrix) with `tx` translation in the x axis, `ty` in the y axis, `tz` in the z axis by calling the `translation_matrix` [`Mat`](#matrix) static function. The result is always a `4x4` translation matrix.
> Example: `Matrix::translation_matrix(tx, ty, tz)`

Similarly, you can get a scale [matrix](#matrix) with `sx` scaling in the x axis, `sy` in the y axis, `sz` in the z axis, by calling the `scale_matrix` [`Mat`](#matrix) static function. The result is also always a `4x4` scaling matrix.
> Example: `Matrix::scale_matrix(sx, sy, sz)`

You can get the 2-dimensional cross product between two vectors by calling the `CrossProduct2D` static [`Mat`](#matrix) function with both vectors as parameters. This function returns a `double`.
> Example: `Matrix::CrossProduct2D(v1, v2)`

Similarly, you can get the 3-dimensional cross product by calling the `CrossProduct3D` static [`Mat`](#matrix) function. This function returns a `4x1` [`Mat`](#matrix) (vector).
> Example: `Matrix::CrossProduct3D(v1, v2)`

# Engine
The [`Engine`](#engine) class is a singleton which is responsible for handling the setup, SDL for direct rendering (pixel wise through buffers), the drawing routines, as well as some math related functions (such as clipping), [event handling](#[events](#events)), storing and managing the [window manager](#window-manager), and cleaning everything. It holds everything together and controls the flow of the program, which is explained further in the [main.cpp](#main-cpp) section.

Its member variables are:<br>
`window`: A pointer to an `SDL_Window` instance.<br>
`renderer`: A pointer to an `SDL_Renderer` instance.<br>
`texture`: A pointer to an `SDL_Texture` instance.<br>
`event`: An `SDL_Event` instance.<br>
`pixel_buffer`: A pointer to the 32-bit `RGBA` pixel buffer array.<br>
`depth_buffer`: A pointer to the `double` depth buffer array, which stores the `view space` depth values for each pixel in the scene.<br>
`CLEAR_COLOR`: The `SDL_RenderClear` color.<br>
`title`: Title of the window to be rendered.<br>
> Default is "Renderer"

`WIDTH`: Width of the window to be rendered.

> Default is `800`

`HEIGHT`: Height of the window to be rendered.<br>

> Default is `600`

`window_manager`: The [window manager](#window-manager).

The `setup` member function initializes `SDL`, the [window manager](#window-manager), and the `pixel` and `depth` buffers.<br>
The `handle_events` member function handles the [events](#events), and is further explained in the [Events](#[events](#events)) section.<br>
The `draw` member function starts the drawing process. It resets the pixel and depth buffers, goes through each [instance](#instance) in the [scene](#scene), and draws them if they are supposed to be shown, by calling the subsequent functions, starting with the `draw_instance` function for the given [instance](#instance). It also draws the transform axes if the given [instance](#instance) has one attached to it.<br>
The `draw_instance` function simply calls the `draw_mesh` function in order to draw the [mesh](#mesh).<br>
The `draw_mesh` function gets the [mesh](#mesh), goes through all of the [mesh's](#mesh) faces, gets their indices, normals, and vertices, then calls `draw_quad` if the face is a [quad](#quad) (has 4 vertices), or `draw_triangle` if the face is a [triangle](#triangle) (has 3 vertices).<br>
The `draw_quad` function simply calls `draw_triangle` twice, by splitting the [quad](#quad) in 2 [triangles](#triangle).<br>
The `draw_triangle` function handles most of the graphics logic:
  1. Converts the vertices into `world space`
  2. Converts them to `view space`
  3. Clips them against all of the frustum's 6 planes
  4. Applies the perspective projection transformation
  5. Performs perspective division
  6. Performs backface culling
  7. Handles [lighting](#light) logic, which is explained in more detail at the [Light](#light) section
  8. Flips the `x` and `y` coordinates to match SDL's window coordinates
  9. Transforms the vertices to `screen space`/`framebuffer space`, at which point they can be drawn in window coordinates.
  10. Any out of bound coordinates are clipped
  11. If `rasterization` is enabled, the given [triangle](#triangle) is rasterized by calling the `fill_triangle` function.
  12. If `wireframe` is enabled, the lines between the vertices are drawn by calling the `draw_line` function for each line.
<br>

The `draw_line` function draws a line between two given points `(x1, y1)` and `(x2, y2)`.<br>
If the change in `y` is less than the change in `x`, this means that the slope is between `0` and `1`, making the line more horizontal than vertical. In this scenario, it's easier to iterate over the `y` value because it always gives a point on the line with a distinct `x` value, whereas iterating over `x` could give multiple points that share the same `y` value.<br>
On the other hand, if the change in `y` is higher than the change in `x`, this means that the line is more vertical than horizontal, and changing `y` not necessarily results in a point on the line that "sits" on a new `x` value (keep in mind that these values are discrete), so in this case it's better to iterate over the `x` values, which will always give a point at a distinct `y` value.<br>
Each of these pixels are colored, and all pixels in between are also colored, taking into account whether `dy` is greater than `dx` or vice-versa.

> If the absolute difference between `x1` and `x2` is less than `0.5`, a vertical line is drawn at `x1`, iterating over the pixels from the top (lowest `y` value) to the bottom (highest `y` value).

> Depth testing is also performed here, if enabled.<br>

The `fill_triangle` function is responsible for rasterization and shading:
1. Creates and iterates over a bounding box which outlines the given [triangle](#triangle) for each pixel within it.
2. Using barycentric coordinates, it's checked whether the pixel is within the [triangle](#triangle)
3. If the pixel is within the [triangle](#triangle), depth testing is performed (if enabled), and the pixels are colored accordingly.
> More information on the [lighting](#light) behavior and calculations can be found in the [light](#light) section.

> If [lighting](#light) is disabled, all pixels within a [triangle](#triangle) have the same color (the ambient/fill color, which can be seen/modified in the menus, at the [scene tab](#scene-tab)).

The `render` function clears the screen, updates the texture with the `pixel_buffer`, then updates the `renderer`, which is subsequently updated by `ImGUI`, and finally updated through `SDL_RenderPresent(renderer)`.


# Triangle
The smallest logical geometric figure is a [triangle](#triangle). A `Triangle` is simply an object that holds 3 vertices, which in this context are 3 `4x1` vectors. ([`Mat`](#matrix) instances)

The `4x1` dimension is for, respectively, the `x`, `y`, and `z` 2D/3D space coordinates of the vertex. The `w` 4th dimension is used for storing values before transforms as well as allowing for a 1-step translation through [matrix](#matrix) vector multiplication. These are formally called homogeneous coordinates.

There are 2 ways in which you can instantiate a `Triangle`:
1. Calling the constructor `Triangle(vertex_a, vertex_b, vertex_c)`, where `vertex_a` is the [`Mat`](#matrix) instantiated vector, and so on for the other vertices.<br>
2. Calling the constructor `Triangles(vertices)`, where `vertices` is an array of `3` vertex ([`Mat`](#matrix)) instances (i.e. `const Mat[3] vertices`).

# Quad

The second smallest logical geometric figure is a quad, which is implemented as the struct `Quad`. A `Quad` is a figure which consists of 4 vertices, which can be broken down as 2 [triangles](#triangle) (or vice-versa).

There are 4 ways in which you can instantiate a `Quad`:
1. Calling the constructor `Quad(triangle_a, triangle_b)` with both being [`Triangle`](#[triangle](#triangle)) instances.<br>
2. Calling the constructor `Quad(triangles)` where `triangles` is a [`Triangle`](#[triangle](#triangle)) array of size `2` (i.e. `const Triangle triangles[2]`).<br>
3. Calling the constructor `Quad(vertex_a, vertex_b, vertex_c, vertex_d)`, where each one is a vertex [`Mat`](#matrix) vector.<br>
4. Calling the constructor `Quad(vertices)`, where `vertices` is a [`Mat`](#matrix) array of size `4`.

# Mesh

The largest geometric figure is a [`Mesh`](#mesh). It holds information about an arbitrary 3D [mesh](#mesh) which can be loaded from an `obj` formatted file or hardcoded (by, for example, manually defining the vertex array and the face indices).
A face can be made up of either `3` or `4` vertices, so both [triangles](#triangle) and [quads](#quad) are considered faces.

Its member variables are the following:<br><br>
`vertices`: An `std::vector` of type [`Mat`](#matrix), which stores all of the figure's vertices.<br><br>
`faces_indices`: An `std::vector` of type `std::vector<uint32_t>>`, which stores all the face indices.  The reason why there is a nested `std::vector` here is because each individual face is its own `std::vector<uint32_t>` (an array of unsigned integers), which identify the respective vertex by indexing from the `vertices` array.<br><br>
`normals`: The same behavior described above for `vertices`, except that this is meant for normals.<br><br>
`normal_indices`: The same behavior described above for `faces_indices`, except that this is meant for normals.<br><br>
`mesh_filename`: Stores the filename of the [mesh](#mesh), which includes the extension.<br><br>
`mesh_id`: Uniquely identifies the [mesh](#mesh). Throughout the program, this value is set as the number of total [meshes](#mesh) loaded for the current [scene](#scene), `total_ever_meshes`, which then gets incremented by `1`.<br><br>

> **The face indices start at `1`.**<br>

> **The z coordinate for the vertices and normals are flipped for the sake of intuition, as otherwise the world z coordinate would be negative for vertices in front of the [camera](#camera).**<br>


> The `obj` formatted [meshes](#mesh) must be within the given `models_folder`, and `mesh_filename` must be a valid file within that folder.<br>

> The `models_folder` must end with a `\`

You are able to get the number of vertices in a [`Mesh`](#mesh) by calling the member function `total_vertices`, or `total_faces` in the case of faces.

Only the vertex coordinates and normals are loaded from the model file. If not given, each vertex normal is derived at runtime as the average of the normals for the faces that share that vertex. This produces incorrect [lighting](#light) for `Gouraud` and `Phong` shading, so it's preferred to use the included normals. Still, it works fine for non-sharp [meshes](#mesh) such as a cube.

There are 2 ways in which you should instantiate a [`Mesh`](#mesh):

1. Calling the constructor `Mesh()`, which instantiates a hardcoded cube
	> You can find specific values, coordinates, and other values such as the `width`, `height`, and `depth` of the cube within the function definition. The `mesh_filename` will be set to `cube.obj`.
2. Calling the constructor `Mesh(current_scene.total_ever_meshes)`, which simply creates an empty [`Mesh`](#mesh) with the given ID and subsequently increments it
3. Calling the constructor `Mesh(model_path, mesh_filename, current_scene.total_ever_meshes)`, which loads the `obj` file `mesh_filename` in the folder `model_path`
	> `model_path` is the relative or absolute path to the `obj` formatted file<br>
	
# Instance

An [instance](#instance) is pretty self-explanatory. It is simply an [`Instance`](#instance) of some loaded [`Mesh`](#mesh). This is done so that each rendered object in the [scene](#scene) gets its own parameters independent of any other while also discarding the need to duplicate the same [mesh's](#mesh) data over and over when placing it in distinct settings within the scene.
> The [instances](#instances) can have any name, but preferably they should be unique.

Its member variables are the following:<br>

`instance_name`: You can give a name to an [instance](#instance) in order to help you identify it. If no [instance](#instance) name is given, the [`Instance`](#instance) member function `create_instance_nameid` is used in order to create a name for it, which is preceded by its [mesh's](#mesh) filename.<br>
The [instance](#instance) name can be set through the [instances tab](#instances-tab) menu, programmatically by calling `instance.create_instance_nameid(total_ever_instances)`, or by initializing through a constructor with the given [instance](#instance) name.<br><br>
`instance_id`: Uniquely identifies the [`Instance`](#instance). Behaves the same way as `mesh_id`, except that it is used for [instances](#instances).<br>
`[mesh](#mesh)`: A pointer to the [`Mesh`](#mesh) which will be rendered.<br>
`tx`: Translation along the `x` axis (positive values move to the left since it is a z-flipped right-handed coordinate system)<br>
`ty`: Translation along the `y` axis<br>
`tz`: Translation along the `z` axis<br>
`yaw`: Rotation around the `y` axis<br>
`pitch`: Rotation around the `x` axis<br>
`roll`: Rotation around the `z` axis<br>
`orientation`: A [`Quaternion`](#quaternion) which represents the [instance's](#instance) orientation<br>
`sx`: Scaling along the `x` axis<br>
`sy`: Scaling along the `y` axis<br>
`sz`: Scaling along the `z` axis<br>
`TRANSLATION_MATRIX`: A `4x4` translation [matrix](#matrix) which holds an arbitrary translation<br>
`ROTATION_MATRIX`: A `4x4` rotation [matrix](#matrix) which holds arbitrary rotation(s)<br>
`SCALING_MATRIX`: A `4x4` scaling [matrix](#matrix) which holds an arbitrary scale<br>
`MODEL_TO_WORLD`: A `4x4` [matrix](#matrix) which will hold the final transformation [matrix](#matrix) for the given [instance](#instance)<br>
`show`: A `bool` toggle for whether the [`Instance`](#instance) will be rendered or not.<br>
`is_light_source`: Positive when the given [instance](#instance) belongs to a [[light source](#light)](#light), negative otherwise<br>
`is_axes`: Whether the given [instance](#instance) is the transform axes<br>
`has_axes`: Whether the given [instance](#instance) has the transformed axes attached to it<br>

There are so many ways in which you can instantiate an [`Instance`](#instance) (and they are not that much different) that it would be better if you look at the `Instance.h` header yourself. In summary, they revolve around different combinations of the [instance](#instance)'s name, [`Mesh`](#mesh), translation, rotation, scaling parameters, and/or their respective transformation matrices.
> One thing that should almost always be present in the constructor is the [current [scene's](#scene)](#[scene](#scene)) `total_ever_instances` in order to create a unique ID for that [instance](#instance).

# Scene
> The [`Scene`](#scene) configuration file must be in a valid `json` format and within the given `scene_folder`. The `scene_filename` must be a valid file within the folder as well.<br><br>
> The `scene_filename` must include the file extension.<br><br>
> The `scenes_folder` must have a `/` at the end if preferably an `absolute` path. Otherwise you need to figure out the correct file tree for relative indexing.<br><br>
> The `scenes_folder` can be seen and updated in the [scene tab](#scene-tab), as well as the [scene](#scene) name for loading and saving.

A [`Scene`](#scene) instance holds information about the [scene](#scene) to be rendered. This includes the [meshes](#mesh) loaded for the [scene](#scene), as well as the [instances](#instance) and other relevant information.

You can get a copy of a [`Mesh`](#mesh) by calling `scene.get_mesh(mesh_id, mesh)`.
> `scene` is the variable that instantiates your [`Scene`](#scene).<br>
> `mesh_id` is the id of the [`Mesh`](#mesh) you want the copy of.<br>
> `mesh` is the variable which will store the resulting copy if the retrieval is successful.<br>

The same goes for an [`Instance`](#instance), except that the function and parameters are related to the [instance](#instance) - `scene.get_instance(instance_id, instance)`.

> These functions returns `true` if successful, `false` otherwise.

You can get a `pointer` to a [`Mesh`](#mesh) by calling `scene.get_mesh_ptr(mesh_id)`. It works similarly to the above functions, so you can use the respective getter function of the type you need. In this case no reference needs to be passed as an argument, since no copies are made.

> The requested pointer is returned in case of success, or a `nullptr` is returned in the case of failure.

You can also use the names instead of the ids, though there may be a conflict if there are somehow other [meshes](#mesh)/[instances](#instances) with the same name.

Its member variables are:<br><br>
`total_ever_meshes`: Keeps track of the number of loaded [meshes](#mesh). (This is an ever increasing integer which is used for the [mesh](#mesh) ids)<br>
`total_meshes`: Same as `total_ever_meshes`. Currently obsolete.<br>
`total_ever_instances`: Keeps track of the number of [instances](#instances) loaded since the [scene](#scene) has been loaded. (This is an ever increasing integer which is used for the [instance](#instance) ids)<br>
`total_instances`: Keeps track of the number of [instances](#instances) currently in the scene.<br>
`total_triangles`: Keeps track of the number of [triangles](#triangle).<br>
`total_vertices`: Keeps track of the number of vertices.<br>
`rendered_meshes`: Keeps track of the number of rendered [meshes](#mesh)<br>
`rendered_instances`: Keeps track of the number of rendered [instances](#instances)<br>
`rendered_triangles`: Keeps track of the number of rendered [triangles](#triangle)<br>
`rendered_vertices`: Keeps track of the number of rendered vertices<br>
> These variables are mostly obsolete, not properly updated, and currently unused (with the exception of `total_ever_instances` and `total_ever_meshes`)

`scene_meshes`: An `std::deque` of type [`Mesh`](#mesh) which holds the [scene's](#scene) [`Mesh`](#mesh) instances.<br>
`scene_instances`: An `std::deque` of type [`Instance`](#instance) which holds the [scene's](#scene) [`Instance`](#instance) instances.<br>
`scene_name`: Stores the name of the [`Scene`](#scene). As of right know it's left unused, but could eventually be useful for identifying individual [scenes](#scene).<br>
`scene_filepath`: Stores the filepath of the `json` formatted [`Scene`](#scene) configuration file .<br>
`scene_data`: The [scene](#scene) `JSON` data will be stored here. The JSON parser is nlohmann's `nlohmann::json`, which is an external library.

`default_world_up`: The vector that represents the default `world space` up axis.
> Default is (0, 1, 0, 0)

`default_world_right`: The vector that represents the default `world space` right axis.
> Default is (1, 0, 0, 0)

`default_world_forward`: The vector that represents the default `world space` forward axis.
> Default is (0, 0, 1, 0)
> 

`camera`: An instance of the [camera](#camera).<br>
`light_source`: The [scene's](#scene) [light source](#light), which is represented by a [`Light`](#light) instance. As of right now, there can only be a single [light source](#light) in the [scene](#scene), which is managed through this variable, regardless of whether [lighting](#light) is enabled for the scene.

`axes_mesh`: The [`Mesh`](#mesh) which represents the transform axes.<br>
`axes_instance`: The [Instance](#Instance) which represents the transform axes.
> This object is a 3D axes that gets rendered at the same position and orientation as the selected model in the [scene](#scene), so it serves as a reference point to which [scene](#scene) object is selected as well as for the transformations and orientation.
> 
> The [`Mesh`](#mesh) that is loaded for this model is the `axes.obj` file. It should be within the models folder.<br>


`BG_COLOR`: An hexadecimal value representing the [scene's](#scene) background color.
> Default is `0x000000FF`

`LINE_COLOR`: An hexadecimal value representing the [scene's](#scene) line drawing color.
> Default is `0x00FF00FF`

`FILL_COLOR`: An hexadecimal value representing the [scene's](#scene) [triangle](#triangle) fill/rasterization color.
> Default is `0x66285CFF`

The [`Scene`](#scene) instances can be saved to and loaded from configuration files in `json` format. This can be done through the [`Scene`](#scene) tab in the menus or programmatically through the [`Scene`](#scene) member functions `save_scene` and `load_scene`, respectively.

The [`Scene`](#scene) can be instantiated in 4 ways:

1. Calling the empty constructor `Scene()`. In this case the default values are set and the transform axes and [light source's](#light) [mesh](#mesh)/[instances](#instances) would need to be manually loaded.<br><br>
2. Calling the constructor by passing the models folder `Scene(models_folder)`. This creates an empty [scene](#scene) with the transform axes and the [light source](#light) properly loaded and setup.<br><br>
3. Calling the constructor by passing the models folder, rotation orientation, and whether to enable verbose logging `Scene(models_folder, rotation_orientation, verbose`. This handles the transform axes and [light source](#light) setup and additionally adds a cube (the `cube.obj` [mesh](#mesh) in the models folder) to the [scene](#scene), as well as an arrow (`arrow.obj`) for the [light source](#light).<br><br>
4. Calling the constructor by passing the `scene_folder`, `scene_filename`, `models_folder`, `rotation_orientation`, `update_camera_settings`, and `verbose`. This loads the [scene](#scene) from the file `scene_filename` within the folder `scene_folder`, with the models loaded from the folder `models_folder` in the given `rotation_orientation`, using the [camera](#camera) settings given in the [scene](#scene) configuration file if `update_camera_settings` is `true` (resets it to default otherwise), and logs to the console some information if `verbose` is `true`.

A given [`Scene`](#scene) instance can be saved by calling the `save` member function, by passing the folder in which to save it and its filename as arguments.<br>
> Example: `scene.save("D:\\Scenes\\", "saved_scene.json")`

As it stands, a blank [`Scene`](#scene) is created on program startup. It is up to the user to load a blank scene or any given [`Scene`](#scene) through the menu. An error message warning you will be displayed if your [scene](#scene) file or the models within your [scene](#scene) could not be loaded (specifically if the path is incorrect).
> There are 3 types of errors handled when attempting to load a [scene](#scene):
> 1. `SceneError::ModelLoad`: Occurs when the model couldn't be loaded. It can be due to an invalid file format, or more likely due to an incorrect filepath. In the latter case, it can usually be fixed by correctly setting the model folder and the respective model file within it.
> 2. `SceneError::SceneLoad`: Occurs when the [scene](#scene) couldn't be loaded due to an invalid filepath. If that's the case, it can usually be fixed by correctly setting the [scene](#scene) folder and the respective [scene](#scene) file within it.
> 3. `SceneError::JSONParsing`: Occurs when the [scene](#scene) couldn't be loaded due to an invalid JSON format when parsing. This is usually caused by extra or missing commas, braces, brackets, quotation marks, etc... Make sure that the given [scene](#scene) file is in a valid JSON format.

`errored_path`: Holds an string with the path to the file (model or [scene](#scene)) which was attempted to be loaded when an error occurred (empty otherwise).

This is an example of how JSON [scene](#scene) configuration files are formatted (this file can be found within the `scenes` folder in this repo):

**cube.json**
```
{
    "scene": "cube.json",
    "camera": {
        "default_position": {
            "x": 0.0,
            "y": 0.0,
            "z": 0.0
        },
        "translation": {
            "x": 0.0,
            "y": 0.0,
            "z": 0.0
        },
        "default_direction": {
            "x": 0.0,
            "y": 0.0,
            "z": 1.0
        },
        "direction": {
            "x": 0.0,
            "y": 0.0,
            "z": 1.0
        },
        "default_up": {
            "x": 0.0,
            "y": 1.0,
            "z": 0.0
        },
        "up": {
            "x": 0.0,
            "y": 1.0,
            "z": 0.0
        },
        "rotation": {
            "x": 0.0,
            "y": 0.0,
            "z": 0.0
        }
    },
    "bg_color": {
        "r": 0,
        "g": 0,
        "b": 0
    },
    "line_color": {
        "r": 0,
        "g": 255,
        "b": 0
    },
    "fill_color": {
        "r": 102,
        "g": 40,
        "b": 92
    },
    "light": {
        "type": "directional",
        "enabled": true,
        "color": {
            "r": 102,
            "g": 40,
            "b": 92
        },
        "intensity": 1.0,
        "minimum_exposure": 0.1,
        "model": "arrow.obj",
        "position": {
            "x": 0.07,
            "y": 0.15,
            "z": 0.47000000000000003
        },
        "rotation": {
            "x": -28.82203865457232,
            "y": 21.741555656980953,
            "z": 109.75987370009548
        },
        "direction": {
            "x": 0.3245333323392281,
            "y": 0.4820907072649036,
            "z": 0.8137976813493758
        },
        "up": {
            "x": -0.8137976813493751,
            "y": -0.2961981327260267,
            "z": 0.4999999999999952
        },
        "scale": {
            "x": 1.0,
            "y": 1.0,
            "z": 1.0
        }
    },
    "models": {
        "cube.obj": {
            "instances": {
                "1": {
                    "show": true,
                    "translation": {
                        "x": 0.0,
                        "y": 0.0,
                        "z": 1.0
                    },
                    "rotation": {
                        "y": 0.0,
                        "x": 0.0,
                        "z": 0.0
                    },
                    "scale": {
                        "x": 1.0,
                        "y": 1.0,
                        "z": 1.0
                    },
                    "model_to_world": [
                        [
                            1.0,
                            0.0,
                            0.0,
                            0.0
                        ],
                        [
                            0.0,
                            1.0,
                            0.0,
                            0.0
                        ],
                        [
                            0.0,
                            0.0,
                            1.0,
                            1.0
                        ],
                        [
                            0.0,
                            0.0,
                            0.0,
                            1.0
                        ]
                    ]
                }
            }
        }
    }
}
```

Virtually all of these parameters can be ommitted, in which case the values will be set to default. When it comes to the [camera](#camera) and [light source](#light), you can provide only the direction vectors, only the rotation parameters (`yaw`, `pitch`, and `roll`), or both.
> The rotation parameters (`yaw`, `pitch`, `roll`) are described in degrees in the configuration file, but are converted to radians internally

> Keep in mind that the rotation parameters and direction vectors should match, otherwise there will be a conflict

> The vectors do not need to be of unit length and the rotation parameters do not need to be within an specific range

The values preceded by `default_` here are better left unchanged. My idea was that the user could have an arbitrary starting point from the `world space`'s origin and/or orientation, and the values would be infered accordingly. I haven't had the time to test this out properly nor implement it as I wanted to due to time constraints and it's a feature of little relevance that virtually nobody would ever touch, so it should not work as intended currently, although some of its foundation and behavior have been implemented.

The `scene` field is meant to be the name of the [scene](#scene) (not filename), but since no functionality that uses this was implemented it is obsolete.

The [camera](#camera) settings are described in the [`Camera`](#camera) section.
The [light](#light) settings are described in the [Light](#light) section.
The [instance](#instance) settings are described in the [`Instance`](#instance) section.

The `bg_color` setting controls the color of the background.<br>
The `line_color` setting controls the color of the lines drawn.<br>
The `fill_color` setting controls the color of the [triangles](#triangle)/rasterization. (Irrespective of [lighting](#light). This is also called ambient [light](#light))

You can manually create a [scene](#scene) by following this layout, as long as you have the model files and you properly set the models folder. Though it would evidently be tiresome for a large scene.

> The first [instance](#instance) in a [scene](#scene) is always the [light source](#light).

# Camera
The [`Camera`](#camera) is a logical object which stores the [camera](#camera) parameters, transformations, and related functions.

`position`: A `4x1` vector [`Mat`](#matrix) instance that stores the position of the [camera](#camera) in `world space`. The first 3 components are the `x`, `y`, and `z` coordinates, respectively. The 4th component is always `1` and solely used to simplify [matrix](#matrix) operations.<br>

`default_position`: The default position before any translation.
> Default value: (0, 0, 0, 1)

`direction`: A `4x1` vector [`Mat`](#matrix) instance that stores the forward direction of where the [camera](#camera) is pointing. The first 3 components are the `x`, `y`, and `z` coordinates, respectively. The 4th dimension is always `0`.<br>

`default_direction`:  The default direction before any rotation.
> Default value: (0, 0, 1, 0)

`up`: Same as the direction vector, except that it represents the vector orthogonal to the direction vector that points up.

`default_up`: The default up vector before any rotation.
> Default value: (0, 1, 0, 0)

`default_right`: The default right vector (orthogonal to both direction and up vectors) before any rotation.
> Default value: (1, 0, 0, 0)

`orientation`: A [`Quaternion`](#quaternion) which stores the `orientation` of the [camera](#camera), which is essentially the accumulated rotation.<br>
`yaw`: A `double` representing the [camera](#camera) `yaw` in `world space`, as the deviation from the default orientation.<br>
`pitch`: A `double` representing the [camera](#camera) `pitch` in `world space`, as the deviation from the default orientation.<br>
`roll`: A `double` representing the [camera](#camera) `roll` in `world space`, as the deviation from the default orientation.<br>
`VIEW_MATRIX`: A `4x4` [matrix](#matrix) [`Mat`](#matrix) instance that represents the view matrix. This [matrix](#matrix) "converts" the coordinate system from `world space` to `view space` or `camera space`, where objects are seen from the perspective of the [camera](#camera). It is the opposite of the `MODEL_TO_WORLD` [matrix](#matrix) for [instances](#instance).<br>
`VIEW_INVERSE`: A `4x4` [matrix](#matrix) [`Mat`](#matrix) instances which stores the inverse of the view matrix. It is used to convert vertices from `view space` back to `world space` in the pipeline.<br>
`PROJECTION_MATRIX`: A `4x4` [matrix](#matrix) [`Mat`](#matrix) instance that stores the perspective projection.<br>
`SCALE_MATRIX`: A `4x4` [matrix](#matrix) [`Mat`](#matrix) instance which is used for converting models from `NDC (Normalized device coordinate) space` into `Screen space`/`Framebuffer space`, which essentially takes the models from 3D space into 2D space window coordinates.<br>
`AR`: The aspect ratio.
> Default value: `4/3` since the default width is `800` and height is `600`, but as the window gets resized, it gets set to the new width over the new height.

`near`: The position of the `near` plane relative to the [camera](#camera).
> Default is `0.01`

`far`: The position of the `far` plane relative to the [camera](#camera).
> Default is `1000`

`FOV`: The field of view in degrees.
> Default is `60`

`FOVr`: The field of view in radians.
> `FOVr` is what is used internally, and it is dependent on `FOV`, so it is always re-calculated when `FOV` is updated.

The [`Camera`](#camera) can be instantiated with its default values by ommitting the parameters, or you could pass any or all of `WIDTH` (window width), `HEIGHT` (window height), `near`, `far`, `FOV` in degrees.

When the window is resized, the function `update_window_resize` is called in order to update the aspect ratio `AR`, then updates the projection [matrix](#matrix) and scale matrix.

When parameters used in the projection [matrix](#matrix) are updated, the function `update_projection_matrix` is called in order to update it.

Whenever the view [matrix](#matrix) is updated, the function `update_view_inverse` is also called in order to update its inverse.

The function `lookat` with no parameters is called in order to update the view [matrix](#matrix) whenever there is a change to the [camera](#camera), namely the position and/or direction vector(s).

If called with the `target_vector` parameter, it updates the view [matrix](#matrix) to look at the given target vector.

If called with the `position`, `direction`, and `up` parameters, it returns a view [matrix](#matrix) which looks at the `direction` vector, with the given `up` vector, at the given `position`.

If called with the `position`, `direction`, `up`, and `target` parameters, it returns a view [matrix](#matrix) which looks at the given `target` vector, at the given `position`, and updates the given `direction` and `up` vectors.

# Light

A [`Light`](#light) object represents a [light source](#light). Currently, every [scene](#scene) always has one unique [light source](#light), which can be enabled or disabled.

There are 3 different types of shading: `Flat`, `Gouraud`, and `Phong` shading. They are defined in the `ShadingType` enum.
* `ShadingType::Flat`: The simplest, fastest, and least accurate shading. The same color is calculated for the entire [triangle](#triangle), and its `world space` normal is calculated from any of its 3 vertices.
* `ShadingType::Gouraud`: A good shading type, not too slow, but not too accurate. A color is calculated for each vertex separately and interpolated over all pixels within that [triangle](#triangle) when rasterizing.
* `ShadingType::Phong`: The most accurate, but also slowest shading type. An interpolated normal is calculated for each vertex separately during [mesh](#mesh) loading, then interpolated for each pixel during rasterization, at which point the color is calculated for that pixel, for all pixels within that [triangle](#triangle).

A [light source](#light) can be of 3 different types: a `directional`, `point`, or `spotlight`. They are defined in the `LightType` enum:
* `LightType::directional`: Only the direction of the [light source](#light) is taken into account. The position is irrelevant.
* `LightType::point`: Only the position of the [light source](#light) is taken into account. The direction is irrelevant.
* `LightType::spotlight`: Both the [light source](#light) position and direction are taken into account. This is not properly implemented, though.

<br>
Its member variables are:<br><br>

`lighting_type`: The [light source](#light) type
> Default value: `LightType::directional`

`shading_type`: The shading type
> Default value: `ShadingType::Flat`

`position`: A `4x1` vector representing the position of the [light source](#light)
> Default value: `(0, 0, 0, 1)`

`direction`: A `4x1` vector representing the forward axis of the [light source](#light)<br>
`up`: A `4x1` vector representing the vertical axis of the [light source](#light)<br>

`default_direction`: The [light source's](#light) forward axis before any rotation.<br>
> Default value: `(0, 0, 1, 0)`

`default_up`: The [light source's](#light) vertical axis before any rotation.<br>
> Default value: `(0, 1, 0, 0)`

`default_right`: The [light source's](#light) horizontal axis before any rotation.
> Default value: The 3D cross product between the `default_direction` and `default_up` (which for the default values is `(1, 0, 0, 0)`)

`tx`: Translation along the horizontal axis.<br>
`ty`: Translation along the vertical axis.<br>
`tz`: Translation along the forward axis.<br>
`yaw`: Rotation around the vertical axis.<br>
`pitch`: Rotation around the horizontal axis.<br>
`roll`: Rotation around the forward axis.<br>
`orientation`: A [`Quaternion`](#quaternion) representing the orientation/accumulated rotation of the [light source](#light).<br>
`intensity`: The [light source's](#light) base [light](#light) intensity. The higher the brighter the [light](#light) will be and vice-versa.
> Default value: `1`

`minimum_exposure`: The minimum treshold after which the [light](#light) is considered to not reach the point. If the final intensity value in the pipeline is less than this value, the final color is set to the [scene's](#scene) ambient/fill color.
> Default value: `0.1`

`color`: The color emitted by the [light source](#light).
> Default value: `0x66285CFF`

`enabled`: Whether the [light source](#light) is enabled.
> Default value: `true`

`has_model`: Whether the [light source](#light) has a 3D model (this can also be used to hide/show the 3D model without disabling the [light source](#light)).
> Default value: `false`

`mesh`: A [`Mesh`](#mesh) representing the [light source](#light).
> Default value: `nullptr`

`instance`: An [`Instance`](#instance) representing the [light source](#light). This is a [scene](#scene) object.
> Default value: `nullptr`

> If you want a [scene](#scene) object representing the [light source](#light), you should check the `Has model` checkbox, then choose/load a [`Mesh`](#mesh) for the [light source](#light).



The [light](#light) function is calculated as `intensity(similarity, base_intensity, attenuation) = similarity * base_intensity * attenuation`.
> `similarity`: The angle between the [light source](#light) and a surface normal.<br>
> `base_intensity`: A base value for the [light](#light) intensity, which the user can control through the [light tab](#light-tab).<br>
> `attenuation`: A function for adjusting [light](#light) according to the distance between the [light source](#light) and a vertex. The attenuation function is `attenuation(distance) = 1 / (distance * distance)`.<br>
> `distance`: The distance between the [light source](#light) and the vertex.

The attenuation function is ignored if the [light](#light) type is `directional`, as the position of the [light source](#light) does not influence the [light](#light) intensity. In this scenario, the `similarity` value is calculated as the angle between [light](#light) direction and a vertex's `world space` normal.

If it's a `point` [light](#light), `similarity` is calculated as the angle between a vertex in `world space` and the vector from the [light source](#light) to the given [triangle](#triangle).

If it's a `spotlight`, both are calculated and multiplied together.

The final color is interpolated between the [light](#light) color and the ambient [light](#light) color for the [scene](#scene), by taking into account the [light](#light) intensity.

If the final [light](#light) intensity is below the `minimum_exposure` treshold, which can be controlled through the [light tab](#light-tab), or it does not face the [light source](#light), then the color is set to the ambient [light](#light) for that [triangle](#triangle)/vertex/pixel (not the [light source's](#light) color).

The `world space` normals are calculated with the [`Mat`](#matrix) `CrossProduct3D` function, by getting the line vectors for each side of the [triangle](#triangle).

The normals, pixels, and depth values are interpolated using barycentric coordinates.

A [`Light`](#light) can be instantiated with no arguments.

# Rotation
> The rotation parameters (`yaw`, `pitch`, `roll`) are described in degrees in the configuration file, but are converted to radians internally. The reason for this is because degrees are more intuitive for user interpretation.

The rotations are defined as positive when counter-clockwise, based on a right-handed coordinate system where the positive z axis points outward. However, as the [camera](#camera) by default points at the positive z axis, I decided to define that the [camera](#camera) rotation parameters (i.e. `yaw`, `pitch`, `roll`) all start at 0, as if the [camera](#camera) were pointing at (0, 0, -1) instead of (0, 0, 1). Depending on the interpretation of the coordinate system this shouldn't be the case, as I purposefully flipped the z axis during [mesh](#mesh) loading for better intuition, as now positive z coordinates are in front of the [camera](#camera).

The rotation order applied is `yaw` first, then `pitch`, then `roll`.

> Yaw: Rotation around the `y` axis. (z-to-x axis)<br>
> Pitch: Rotation around the `x` axis. (y-to-z axis)<br>
> Roll: Rotation around the `z` axis. (x-to-y axis)

If including both a direction and rotation in the [scene](#scene) file, they should logically match.

There are a few implementations for rotation around each axis, as well as rotation matrices, in the [Engine](#engine) class. These are currently unused and legacy as I have since implemented and used [quaternions](#quaternion) for rotation.

Rotations can be done with respect to `local` space or `world` space. Every time you see `orientation` in the code or this README, it must be one of `Orientation::local` or `Orientation::world`. `local` orientation has been more extensively tested and seems to work without issues, whereas while `world` orientation works, it might not be as consistent.

## Quaternion
> Some functionalities do not need to be within this class since they are not related to a [quaternion](#quaternion) at all, but since they represent a rotation and this is an isolated class, it makes sense to leave them here instead of creating a separate file or class for them.

A [`Quaternion`](#quaternion) holds information about a [quaternion](#quaternion) as well as methods related to it. Namely, the `x`, `y`, `z`, and `w` coordinates respectively.

A [`Quaternion`](#quaternion) with `x`, `y`, and `z` equal to `0` and `w` equal to `1` is known as the identity [quaternion](#quaternion), which represents no rotation. These are the default values.

[Quaternions](#quaternion) are used for representing orientations and rotations, encapsulating every change to an object's orientation. They are also used for rotating points, optionally around an arbitrary axis and angle measure.

Each [quaternion](#quaternion) can also be converted into a rotation matrix.

It is also used for deriving the rotation axis between two points, which helps in identifying the difference between two vectors in terms of angles, which is how, for example, given a default vector and a rotated version of that vector, the `yaw`, `pitch`, and `roll` can be extracted from the resulting [`Quaternion`](#quaternion)'s rotation matrix. This can be done through its static member function `GetAnglesFromDirection`.
> Example: `Quaternion::GetAnglesFromDirection(Orientation::local, default_vector, rotated_vector, result_yaw, result_pitch, result_roll)`

It can also derive `yaw`, `pitch`, and `roll` from its own rotation [matrix](#matrix) through its `GetAngles` member function.
> Example: `quaternion.GetAngles(Orientation::local, result_yaw, result_pitch, result_roll)`

You can get a given [`Quaternion`](#quaternion)'s rotation [matrix](#matrix) by calling its member function `get_rotationmatrix`. This returns the `4x4` rotation [matrix](#matrix) represented by that [`Quaternion`](#quaternion).
> Example: `quaternion.get_rotationmatrix()`

You can get the [`Quaternion`](#quaternion)'s complex conjugate (essentially its inverse) by calling the member function `get_complexconjugate`.
> Example: `quaternion.get_complexconjugate()`

You can create a [quaternion](#quaternion) which represents a rotation around an arbitrary axis and angle by calling the static member function `AngleAxis`.
> Example: `Quaternion::AngleAxis(axis_x, axis_y, axis_z, angle)`

You can create a [quaternion](#quaternion) from `yaw`, `pitch`, and `roll` angles by calling the static member function `FromYawPitchRoll`.
> Example: `Quaternion::FromYawPitchRoll(orientation, yaw, pitch, roll, default_x_axis, default_y_axis, default_z_axis)`<br>
> Note: The "default" axes here are the values for each axis in your coordinate system before any rotation is applied to them.<br>
> In the code, they are pretty much always the same as the default world space axes.<br>
> So, usually:<br>
> `default_x_axis`: (1, 0, 0, 0)<br>
> `default_y_axis`: (0, 1, 0, 0)<br>
> `default_z_axis`: (0, 0, 1, 0)

You can rotate points using [`Quaternion`](#quaternion)s in many ways, but you will need either a point, an axis, and an angle to rotate around, or a [`Quaternion`](#quaternion) representing the desired rotation and a point to be rotated.
In order to rotate using an axis and an angle, you can call one of the static member functions `RotatePoint` with the relevant parameters.
> Example: `Quaternion::RotatePoint(point, axis, angle, is_position)`<br>
> `Quaternion::RotatePoint(rotation, point, is_position)`<br>
> Note: `is_position` should be `false` if the 4th dimension of the resulting vector should be `0` (which is always the case for direction vectors, or `true` otherwise)

You can get the [`Quaternion`](#quaternion) as a 4-dimensional vector by calling the member function `get_4dvector`.
> Example: `quaternion.get_4dvector()`

You can also get only the `x`, `y`, and `z` components by calling the member function `get_3dvector`.
> Example: `quaternion.get_3dvector(is_position)`<br>
> Note: The 4th component will be `1` if `is_position` is `true` and `0` otherwise.

You can get the angle of rotation around the arbitrary rotation axis by calling the member function `get_angle`.
> Example: `quaternion.get_angle()`

You can get the norm/magnitude of the [quaternion](#quaternion) by calling the member function `get_magnitude`.
> Example: `quaternion.get_magnitude()`

You can normalize the [`Quaternion`](#quaternion), making it into a unit [quaternion](#quaternion), by calling the member function `normalize` (divides all components by the magnitude).
> Example: `quaternion.normalize()`

[`Quaternion`](#quaternion)s can be added `+`, subtracted `-`, and multiplied `*` by each other.

A [`Quaternion`](#quaternion) can be instantiated through an empty constructor `Quaternion()`, in which case the `x`, `y`, and `z` components are `0` and `w` is `1`, which represents an identity quaternion.

It can also be instantiated by calling `Quaternion(x, y, z, w)` with the respective values.

# Windows and tabs (menus)
Most settings are controlled, set by, and retrieved from the menus.

## Window manager

The `WindowManager` is a singleton which manages all [`Window`](#window)s. It can, theoretically, hold multiple [`Window`](#window) instances, and set one target window to be drawn (only one is drawn at a time), through the `target_window` member variable. It also is responsible for initializing some `ImGUI` related routines and eventually cleaning them.

Since there is only one [window](#window) - the [general window](#general-window) - the `target_window` is always the `general_window`.

## Window
A [`Window`](#window) is a class which represents an `ImGUI` window, or tab. It is a base class which gets inherited by every window/tab.

All share 3 variables:<br>
`window_alpha`: The alpha/opacity/transparency value for the given [`Window`](#window).<br>
`zero`: A constant value of `0`. This is used solely for the minimum value treshold on certain `ImGUI` widgets, as it expects an `lvalue` (pointer).<br>
`one`: Same as `zero`, except it is a constant value of `1`.

All [`Window`](#window) objects have a `draw` function. This is the function that gets called for all existing visible [`Window`](#window)s and tabs, in order to draw them.

## General window

![general_window](https://github.com/user-attachments/assets/d42a79bb-3911-4367-a7bc-b343cf6107ab)

Currently, there is only one [`Window`](#window) in the program, which is called [`GeneralWindow`](#general-window). Despite that, as mentioned above, tabs within that [`Window`](#window) also inherit from the [`Window`](#window) base class despite not actually being windows, since the base functionality is the same.

The [`GeneralWindow`](#general-window) is the [`Window`](#window) in which most variables are stored and shared between the menus/user interface and other parts of the program.

It has two member variables:<br>
`settings_tab`: The [`SettingsTab`](#settings-tab), which is the settings tab and is explained in more detail in the [Settings tab](#settings-tab) section below.<br>
`scene_tab`: The [`SceneTab`](#scene-tab), which is the [scene](#scene) tab and is explained in more detail in the [Scene tab](#[scene](#scene)-tab) section.

Its `draw` function creates both tabs in the menu and draws them if they are opened (by calling the respective tab(s) `draw` function).

It can be initialized with no arguments.

### Settings tab
![settings_tab](https://github.com/user-attachments/assets/609caf0b-f731-44c6-a1af-a2da163c52dd)

The [`SettingsTab`](#settings-tab) is a tab with some settings that are more closely related to the graphics engine as "global" settings than the [scene](#scene), which is why I separated them.

These are its member variables:<br>
`wireframe_render`: Enables/disables line drawing between the vertices in a [triangle](#triangle).<br>
`rasterize`: Whether to rasterize/fill the [triangles](#triangle)/pixels with color.<br>
`shade`: When enabled alongside `rasterize`, rasterization is done by taking into account the user given [scene's](#scene) ambient/fill color and the [light](#light) as well as its shading (if enabled as well).<br>
`depth_test`: Enables/disables depth testing.<br>
`backface_cull`: Enables/disables backface culling.<br>
`FPS_LIMIT`: The FPS gets capped if it goes above this value. (Rendering is delayed by the timing difference)<br>
`MSPERFRAME`: Milliseconds that should be taken in order to render 1 frame at the given `FPS_LIMIT`.<br>
`framerate`: The current framerate. (This gets averaged at runtime)<br>
`fps_update_interval`: How often, in milliseconds, does the `framerate` get averaged and updated.<br>
`z_fighting_tolerance`: Tolerance value for avoiding z-fighting.<br>
> Default is `0.994` and from my tests seems to work well.

`general_window_opacity`: A pointer to the [general window](#general-window)'s opacity in order to be controlled here by the user.

The `draw` function draws all of this data in an accessible way to the user for interpretation and management through the menu.

The menus are pretty self-explanatory and some settings have a tooltip if you hover over them. You can also reference the above description of the member variables for clarification.

It can be initialized with the pointer to the [general window's](#general-window) opacity.

### Scene tab
![scene_tab](https://github.com/user-attachments/assets/1dc308b1-4896-4d62-9be9-322c8ded0001)

The [`SceneTab`](#scene-tab) contains the [scene](#scene) as well as metadata directly related to it. It also contains the [camera tab](#camera-tab), [instances tab](#instances-tab), and [light tab](#light-tab).

These are its member variables:<br>
`models_folder`: The absolute path to the models folder.<br>
`scene_folder`: The absolute path to the [scene](#scene) folder.<br>
`scene_load_name`: The name of the [scene](#scene) to be loaded.<br>
`scene_save_name`: The name of the [scene](#scene) to be saved.<br>
`real_translation_speed`: Translation speed of the [camera](#camera) by keyboard movement when the window is being shown. This can be modified in the [camera tab](#camera-tab).<br>
`real_rotation_speed`: Same as `real_translation_speed`, but for rotation.<br>
`menu_translation_speed`: Translation speed of dragging the translation sliders in the menus. It can be modified in all of [camera tab](#camera-tab), [instances tab](#instances-tab), and [light tab](#light-tab).<br>
`menu_rotation_speed`: Same as `menu_translation_speed`, but for rotation.<br>
`menu_scaling_speed`: Same as `menu_translation_speed`, but for scaling.<br>
`current_scene`: The current scene.<br>
`rotation_orientation`: The current rotation orientation. It's set to `local` by default, and can be modified in the [instances tab](#instances-tab), [light tab](#light-tab).<br>
`update_camera_settings`: Whether to update the [camera](#camera) settings to the loaded [scene's](#scene) [camera](#camera) settings.<br>
`is_instances_open`: Whether the [instances tab](#instances-tab) is open.<br>
`is_light_open`: Whether the [light tab](#light-tab) is open.<br>
`show_transform_axes`: Whether the transform axes will be rendered. It can be enabled/disabled in the [instances tab](#instances-tab) and [light tab](#light-tab).<br>
`camera_tab`: A pointer to the [camera tab](#camera-tab).<br>
`instances_tab`: A pointer to the [instances tab](#instances-tab).<br>
`light_tab`: A pointer to the [light tab](#light-tab).

Some other variables are placeholders for displaying text and controlling certain `ImGUI` behavior.

You can create a new blank [scene](#scene) by clicking the `New` button.
You can choose your [scene](#scene) folder (where your [scene](#scene) configuration file is) and models folder (where the 3D `obj` formatted model files for the given [scene](#scene) are), by clicking the `Browse` buttons next to them.<br>
You can load a [scene](#scene) within your [scene](#scene) folder by typing its filename into the text box next to the `Load` button, then clicking the button.<br>
Similarly, you can save the current [scene](#scene) to your [scene](#scene) folder by typing the filename in the text box then clicking the `Save` button.

The `Background color` picker controls the color of the background.<br>
The `Line/wireframe color` picker controls the color of the lines drawn.<br>
The `Fill/ambient color` picker controls the color of the [triangles](#triangle)/pixels (irrespective of the [light](#light) color, this is essentially the object's default "real" color).

The [scene](#scene) tab can be initialized with no parameters, and it initializes on its own the [camera tab](#camera-tab), [instances tab](#instances-tab) and [light tab](#light-tab).

#### Camera tab
![camera_tab](https://github.com/user-attachments/assets/397ac8a2-c057-4c4b-b70c-f42212eaac8b)

This tab controls/displays the [camera](#camera) settings. Most of its members are placeholders for displaying things. It also has a pointer to the [`SceneTab`](#scene-tab), in order to access other settings.

The menu settings are pretty self-explanatory.

It can be initialized by passing the [scene tab](#scene-tab) pointer.

#### Instances tab
![instances_tab1](https://github.com/user-attachments/assets/7fd506c7-bffd-4116-add3-c0498550f2ea)

![instances_tab2](https://github.com/user-attachments/assets/8385545a-1efa-4161-bda4-e07451d56b56)


This tab controls/displays the [instances](#instance) in the scene.

You can add a new [instance](#instance) to the [scene](#scene), add a new [mesh](#mesh), and transform any existing [instance](#instance) as you desire. You can, for example, delete [instances](#instances), make them invisible/visible, translate them, rotate them, scale them, etc...

If you want to modify any [instance](#instance), you first have to select it in the list of [instances](#instances).

The menu is pretty self-explanatory.

If enabled, the transform axes are rendered over the [instance](#instance) in the [scene](#scene) once this tab is opened.

#### Light tab

![light_tab1](https://github.com/user-attachments/assets/520d15a1-a86f-4952-996c-f09fa16c1989)

![light_tab2](https://github.com/user-attachments/assets/3b04dabc-b4ec-45ed-93d9-9e9c82039653)

This tab controls/displays the [light](#light) settings. Most of its member variables are placeholders for displaying things. It also has a pointer to the [`SceneTab`](#scene-tab), in order to access other settings.

If enabled, the transform axes are rendered over the [light](#light) in the [scene](#scene) once this tab is opened.

# Events

The [events](#events) are handled in the function `handle_events`, which is an [`Engine`](#engine) member function. It is implemented separately in the file `Events.cpp`.

Here are the implemented key [events](#events):<br>
`1`: Toggles the [general window](#general-window) (`ImGUI` menu).<br>
`W`: Moves the [camera](#camera) forward<br>
`S`: Moves the [camera](#camera) backward<br>
`A`: Moves the [camera](#camera) left<br>
`D`: Moves the [camera](#camera) right<br>
`Up arrow`: Moves the [camera](#camera) up<br>
`Down arrow`: Moves the [camera](#camera) down

Mouse motion is also caught here for rotating the [camera](#camera) with respect to mouse movement.

You can resize the window by clicking and dragging the edges.

You can quit by clicking on the `X` button in the GUI.

# main.cpp
> The default variables, objects, and values are mostly set through the [window manager](#window-manager)


This is where the `main` function is, and where you use the [`Engine`](#engine) instance in order to control the flow of the program.

Here's how it goes in a few steps:
1. Instantiate the [`Engine`](#engine) class, in this case that is the `engine` variable.
2. Run the [`Engine`](#engine) member function `setup`, in order to setup the [`Engine`](#engine), which initializes `SDL`, `ImGUI`, the [window manager](#window-manager), and allocates a pixel and depth buffer.
3. Set a starting [`Scene`](#scene) to be rendered. You can do so by assigning the `engine.current_scene` variable to a [`Scene`](#scene) instance. The [`Scene`](#scene) constructor variables are explained in [Scene](#[scene](#scene)). i.e. `engine.current_scene = Scene(...)`
4. Run the main execution loop.
	1. The `SDL` and `ImGUI` [events](#events) are processed, by calling the [`Engine`](#engine) member function `handle_events`.
	2. The [`Scene`](#scene) [instances](#instances) (and optionally the transform axes) are drawn to the pixel and depth buffers.
	3. Starts the `ImGUI` frame, draws some text, and draws the `ImGUI` windows if they are toggled by pressing `1`.
	4. Renders the new frame by calling the [`Engine`](#engine) member function `render`. Through `SDL` routines, it clears the `renderer`, updates the `texture` with the new pixel buffer, then updates the screen.
	5. If the rendering was too fast, meaning that it took less milliseconds per frame than the given framerate limit should, then the rendering of the next frame is delayed by the difference between the two values.
	
	6. The averaged fps over the given update interval (in milliseconds) is updated, then displayed and printed to the console.
	7. Goes back to the beginning of the loop...

> You can change/update most of these variables in the menus.



# Utils

The file `Utils.h` contains the implementation of some utility functions.

`normalize`: Converts a value within some range to some other range. So if you have the number `50` within the range from `0` to `100`, and you want it in the range `0` to `1`, the function returns `0.5`.<br>
The arguments to the function are, respectively, the previous value which will be converted, the previous minimum value in that range, the previous maximum value in that range, the new minimum value for the new range, and the new maximum value.
> Example: `Utils::normalize(50, 0, 100, 0, 1)` returns `0.5`.

`round_to`: Rounds the given number to an arbitrary number of decimal places. It simply multiplies the given number by 10 to the given number of decimal places, runs the `cmath` standard library `round` function, and divides back by 10 to the number of decimal places.
> Example: `Utils::normalize(3.1415, 2)` returns `3.14`<br>
> `Utils::normalize(3.1415, 0)` returns `3`

`clamp`: Clips a value to a given range. If it is smaller than the minimum value in the range, it gets set to that value, and vice-versa if it is higher than the maximum value. Otherwise it doesn't change.
> Example: `Utils::clamp(-2, 1, 100)` returns `1`

# Implementation details
> The default variables, objects, and values are mostly set through the [window manager](#window-manager)

The renderer uses 4 coordinate systems, of which only the last one is different.

The first 3 coordinate systems represent 3 dimensional space, and are all right-handed. Here, the positive x-axis points to the right, the positive y-axis points up, and the positive z-axis points outward (out of the screen/toward the viewer).

Firstly there is `local space`, which is essentially a 3D model's local coordinate system before they are added to the scene. If you have worked with modelling software like Blender for example, you can think of everything within the Blender [scene](#scene) as being in `local space`.

Secondly there is `world space`, which, as the name implies, is the space which holds all the 3D models present in the [scene](#scene), in absolute values. Each model present in the [scene](#scene) has a transformation [matrix](#matrix) which transforms all the vertices for that [`Instance`](#instance)'s 3D [mesh](#mesh) from `local space` to `world space`, essentially moving it to the [scene](#scene), including the relevant transformations.<br>
The [matrix](#matrix) responsible for this conversion is called `MODEL_TO_WORLD` and every [`Instance`](#instance) has it. This [matrix](#matrix) is a combination of 3 other matrices: first the `SCALING_MATRIX`, secondly the `ROTATION_MATRIX`, then finally the `TRANSLATION_MATRIX`.<br>
These matrices are built with the individual values for each [`Instance`](#instance) and finally combined to make the `MODEL_TO_WORLD` matrix.

> In a right-handed coordinate system, the `z` axis points "out of the screen/toward the viewer". This means that for a [mesh](#mesh) to be visible it would need to have a negative `z` coordinate in world space (assuming a default [camera](#camera) orientation). However, during [mesh](#mesh) loading I purposefully flip it so that a positive `z` value means "farther", which is more intuitive but less semantically accurate. The coordinate system remains the same however, as you can think of it as a right-handed coordinate system rotated by 180 degrees.

Thirdly, there is `view space` or `camera space`. It represents a "view" into the world from the [camera](#camera)'s point of view. It essentially contains all transformations relative to the [camera](#camera). This transformation is stored in the `VIEW_MATRIX`. It can also be seen as a transformation that is the inverse of the `world` transform, because, relative to the [camera](#camera), whenever it moves to the right, the object in front of it appears to move to the left, which means that whatever happens to a logical [camera](#camera) can be simulated by performing the inverse operations to each vertex. <br>The `VIEW_MATRIX` is constructed from a position vector and a direction vector through the [camera's](#camera) `LookAt` function, and it encapsulates the translation and rotation (akin to what happens above for [instances](#instances) in `world space`).

After this, geometric clipping is performed for each [triangle](#triangle) against each of the [camera's](#camera) 6 planes in `view space` (near, far, left, right, top, and bottom planes).

Now perspective projection is applied to the vertices. At this stage the coordinate system is considered to be in `clip space`.
> Note that the `z` transformation here is non-linear.

Finally, for the 3-dimensional space, perspective division is performed over all vertices by their `w` (which is simply their `z` coordinate prior to the projection).
We are now at what is called `NDC (Normalized Device Coordinates) space`, where, theoretically, every point/vertex that made it to this stage is viewable through the [camera](#camera) and its boundaries are in the ranges `[-1, 1]` for the x-axis, `[-1, 1]` for the y-axis, and `[0, 1]` for the z-axis.

Here, a vertex whose `z` coordinate is the same as the `near` plane value would have a `z` of 0, and if it were to be the same as the `far` plane, it would have a `z` of 1.

We can then start processing the vertices.<br>
Here `backface culling` is performed, as well as some [light](#light) shading (if `Flat` shading it is done entirely here).<br>
If `Gouraud` shading, the vertex colors are calculated here for each vertex (they get interpolated at the pixel level later).<br>
If `Phong` shading, the vertex normals are calculated for each vertex (these also get interpolated later).

The last coordinate system is `screen space` or `framebuffer space`. Here the vertices' `x` and `y` coordinates are flipped and scaled by the [camera](#camera)'s `SCALE_MATRIX`.<br>

The `y` axis is flipped because the `y` coordinate increases as you go down when it comes to pixels, but in 3D space it increased as it went up.<br>
As for the `x` axis, since I flipped the `z` coordinates when loading the [meshes](#mesh), the resulting logically and semantically correct right-handed coordinate system with this configuration has the positive `x` axis to the left (not to the right), which means that going to the right would instead decrease the `x` coordinate for the pixel.<br>

Now we are finally in the 2-dimensional space which represents the window coordinates.

> The winding order is counter-clockwise. (The order for drawing the vertices)

If drawing the lines between vertices, which can be toggled through the `wireframe` checkbox in the [settings tab](#settings-tab), or programmatically through the `draw_outline` function parameter, the relevant lines are drawn, with the given `Line color` in the [Scene tab](#[scene](#scene)-tab) or programmatically through the `outline_color` parameter.

If filling/rasterizing [triangles](#triangle), it uses the `Fill color` in the [Scene tab](#[scene](#scene)-tab) or programmatically through the `fill_color` parameter. This is where the vertices and [triangle](#triangle) coordinates are interpolated, and where the fragment shader/texturing/coloring should be implemented.

# Testing
Testing is implemented with `Catch2`.

I implemented some testing in the beginning, but I forgot about it after a while and implementing the main functionality was too time consuming on itself, and I changed everything multiple times, so I eventually gave up on it.

Currently the test covers only most of the [`Mat`](#matrix) [matrix](#matrix) class and `Util` utility functions.

`TestMain.cpp` sets up and runs the test, it contains the `main` function.<br>
`TestMatrix.cpp` contains [matrix](#matrix) related tests.<br>
`TestUtils.cpp` contains utility related tests.

# Notes

- Window is resizeable and the renderer is updated accordingly

- Models are expected to be right-handed, y-up and positive z toward the viewer (out of the screen/negative coordinates)

# Todo

- Use CUDA/ROCm for GPU processing

- Implement element-wise addition and subtraction by scalar values, as can already be done with multiplication and division

- Let user load [meshes](#mesh) with the same name

- Let user delete [meshes](#mesh)

- Improve logging and debugging capabilities

- Improve exception handling

- Include sub-[meshes](#mesh) found in [meshes](#mesh) (`.obj` files can have multiple different parts that comprise the [mesh](#mesh). For example, if modeling a house, you might have separate [meshes](#mesh) for the walls called `Wall_0` or `Floor_2` and so on.)

- Fix spotlight [light](#light) type behavior

- Implement textures

- Implement shaders

- Implement backface culling for non-visible objects from the [camera](#camera)

- Implement collision/physics

- Make sure it works cross-platform and create a proper environment for easy setup

- Remove and/or decrease redundancies

- Broader test coverage

- Optimize and refactor the code in general

- Allow user to visually see the normals on the [scene](#scene) (similar to how it's done with the transform axes)

- Implement SIMD instructions

- Add option to attach the [camera](#camera) to an [instance](#instance) (i.e. to a character or a [scene](#scene) object)

- Implement particles and particle simulation (i.e. water and/or waves simulation, sparks, fire, etc...)

- Implement animations

- Implement multi-threading

- Swap division for multiplication where applicable and intuitive, as it is a less expensive operation in terms of clock cycles

- Combine view and perspective projection matrices for less overhead and wherever possible

- Include and allow user to load/save quaternions into/from [scene](#scene) configuration file

- Write a helper function to decompose matrices (i.e. rotation [matrix](#matrix) -> yaw, pitch, roll or translation [matrix](#matrix) -> tx, ty, tz or view [matrix](#matrix) -> everything, etc...)

- Properly deal with default values different than the default ones. (i.e. axes which don't start at the pre-defined values)

- Unflip the z when loading the [meshes](#mesh) and instead set the default [camera](#camera) direction to (0, 0, -1) instead of flipping the z during [mesh](#mesh) loading and the default [camera](#camera) direction to (0, 0, 1). This would be more semantically accurate to the coordinate system and possibly avoid confusion if manipulating vertices, but also less intuitive for the users.

- Properly handle the edge case for when vectors are antiparallel (point in opposite directions) within the 3D cross product function

- Have a configuration file generated/updated whenever the user closes the program so that everything doesn't need to be re-set on the next run. Also include a "Reset" button on the menu in order to reset the settings to the default settings.

- Add a color attribute to each [instance](#instance) so that they can have their individual colors

- Implement a [scene](#scene) graph for attaching multiple [instances](#instances)/[meshes](#mesh) to a single [instance](#instance)/[mesh](#mesh) (i.e. a character [instance](#instance) may have arms, legs, and so on...). This can be used for grouping [instances](#instances)/[meshes](#mesh) into one, and can also be useful for animations.

- Move drawing and graphics related functions to a separate file `Graphics.cpp`?

- Properly track [scene](#scene) metadata such as number of loaded [meshes](#mesh), [instances](#instances), as well as rendered [triangles](#triangle), lines, etc...

- Camera and world axes are sometimes used interchangeably in the code, but they are different in theory (although not in practice, since the coordinate systems are the same with the default values). I should rename such variables to properly reflect this distinction.
