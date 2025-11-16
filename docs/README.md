# OctTree

Original:

https://www.gamedev.net/articles/programming/general-and-gameplay-programming/introduction-to-octrees-r3529/

Russian translation:

https://habr.com/ru/articles/334990/


# Geometry

## Primitives

### Line

Equation is:

$$x(t) = p + t \cdot \vec{d}$$

+ Point p
+ Vector d

### Plane

Equation is:

$$ \vec{n} \cdot (x - p) = 0$$

+ Point p
+ Vector n

Other form:

$$ \vec{n} \cdot x = s$$

+ double s - scalar, $$s = \vec{n} \cdot p$$

### Segment

TODO

## Objects

### Triangle

By three Points: A, B, C

Plane for triangle by 3 points: 

$$\vec{n}(a,b,c)$$
$$ a =(y_2 - y_1)(z_3-z_1) - (z_2 - z_1)(y_3-y_1)$$
$$ b =(z_2 - z_1)(x_3-x_1) - (x_2 - x_1)(z_3-z_1)$$
$$ c =(x_2 - x_1)(y_3-y_1) - (y_2 - y_1)(x_3-x_1)$$

Point p = A(or B,C)

## Intersection

### 2 Planes

Let the two planes be:

$$ \mathcal{P}_1 : \vec{n}_1 \cdot \vec{X} = s_1 $$
$$ \mathcal{P}_2 : \vec{n}_2 \cdot \vec{X} = s_2 $$

where  $$\vec{n}_1, \vec{n}_2$$ are normal vectors.

---

#### Planes are parallel but distinct

Normals are **parallel**:

$$
\vec{n}_1 \times \vec{n}_2 = \vec{0}
$$

but **not proportional** in offset:

$$
\frac{s_1}{\|\vec{n}_1\|} \ne \frac{s_2}{\|\vec{n}_2\|}
$$

**No intersection.**

they are parallel and separate.


#### Planes coincide

Normals are parallel and offsets match:

$$
\vec{n}_1 \times \vec{n}_2 = \vec{0}, \qquad \frac{s_1}{\|\vec{n}_1\|} = \frac{s_2}{\|\vec{n}_2\|}
$$

Every point on one plane lies on the other.


#### Planes intersect in a line

If normals are **not parallel**:

$$
\vec{n}_1 \times \vec{n}_2 \ne \vec{0}
$$

then the intersection is a **line**.

- **Line direction:**

$$
\vec{d} = \vec{n}_1 \times \vec{n}_2
$$

- **Point on the line:**

  Assume

$$
\vec{P} = a \vec{n}_1 + b \vec{n}_2
$$

  Plug into both plane equations:

1.

$$
\vec{n}_1 \cdot \vec{P} = s_1
$$

2.

$$
\vec{n}_2 \cdot \vec{P} = s_2
$$

  Solving for $$a$$ and $$b$$:

$$
a = \frac{s_2(\vec{n}_1 \cdot \vec{n}_2) - s_1 \|\vec{n}_2\|^2}{(\vec{n}_1 \cdot \vec{n}_2)^2 - \|\vec{n}_1\|^2 \|\vec{n}_2\|^2}
$$

$$
b = \frac{s_1(\vec{n}_1 \cdot \vec{n}_2) - s_2 \|\vec{n}_1\|^2}{(\vec{n}_1 \cdot \vec{n}_2)^2 - \|\vec{n}_1\|^2 \|\vec{n}_2\|^2}
$$

**Intersection line equation:**

$$
\vec{X}(t) = (a \vec{n}_1 + b \vec{n}_2) + t (\vec{n}_1 \times \vec{n}_2)
$$

---

### 2 Lines


#### 1) Parallel
Two lines parameterized as:

$$
\mathbf{x}(t)=\mathbf{p}+t\,\mathbf{d}
$$

$$
\mathbf{y}(s)=\mathbf{q}+s\,\mathbf{e}
$$

Define vectors:

$$
\mathbf{n}=\mathbf{d}\times\mathbf{e}
$$

$$
\Delta=\mathbf{q}-\mathbf{p}
$$

##### 1a) Colinear (same geometric line)
Conditions:

$$
\mathbf{n}=\mathbf{0}
$$

$$
\Delta\times\mathbf{d}=\mathbf{0}
$$

Result: infinitely many common points.

##### 1b) Parallel and distinct
Conditions:

$$
\mathbf{n}=\mathbf{0}
$$

$$
\Delta\times\mathbf{d}\neq\mathbf{0}
$$

Result: no intersection.

#### 2) Not parallel

##### 2a) Skew
Non-coplanarity condition:

$$
\Delta\cdot\mathbf{n}\neq 0
$$

Result: no intersection.

##### 2b) Proper intersection (one point)
Coplanarity and non-parallel conditions:

$$
\mathbf{n}\neq \mathbf{0}
$$

$$
\Delta\cdot\mathbf{n}=0
$$

Intersection parameters:

$$
t=\frac{\big(\Delta\times \mathbf{e}\big)\cdot \mathbf{n}}{\|\mathbf{n}\|^2}
$$

$$
s=\frac{\big(\Delta\times \mathbf{d}\big)\cdot \mathbf{n}}{\|\mathbf{n}\|^2}
$$

Intersection point:

$$
\mathbf{X}=\mathbf{p}+t\,\mathbf{d}=\mathbf{q}+s\,\mathbf{e}
$$

### 2 Segments

TODO

### Line & Triangle

TODO

### 2 Triangles

TODO