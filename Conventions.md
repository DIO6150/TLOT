# Coding Conventions

## Naming

### Files

We use ```PascalCase``` for files :

```markdown
AssetManager.cpp
AssetManager.hpp
```

Extensions are ```.cpp``` and ```.hpp```

### Classes

We use ```PascalCase``` for classes :

```cpp
class AssetManager
{

};
```

### Variables

Variables are to be in ```camelCase``` :

```cpp
double deltaTime;
```

### Member Variables

Member variables must append an underscore after their name :

```cpp
class Entity
{
public:
	int currentHp_;
	int maxHp_;
};
```

### Methods

Methods are to be in ```camelCase``` :

```cpp
double getDeltaTime();
```

## Brackets

Curly brackets must be on the next line :
```cpp
class AssetManager
{
public:
	void onUpdate()
	{
		if (alive_ == true)
		{

		}
	};
};
```

Opening Brackets must follow the previous character unless its from a control block keyword :
```cpp
void onUpdate()
{
	if (alive_ == true)
	{
		while (alive_)
		{
			currentHp_ -= 1;
			updateAliveStatus();
		}
	}
}
```

## Miscellanious

> Try to be as "airy" as possible. Less clutter means more times spent on finding what you need.

> Try to align stuff that repeat on multiple lines by putting appropriate number of spaces between tokens :

```cpp
m_geometry  .create(vertexTemplate);
m_matrixSSBO.create(GL_DYNAMIC_DRAW, 1, 10000000);
m_indexSSBO .create(GL_DYNAMIC_DRAW, 0, 10000000);
```

> However it does not look good most of the time when aligning functions declarations. You can get away by only doing it on public api, just for redability. (this is not really enforced)

```cpp
void uploadMatrix4(std::string key, glm::mat4 const & matrix) const;
void upload1i     (std::string key, int i) const;
void upload3fv    (std::string key, glm::vec3 vec) const;
void upload1f     (std::string key, float f) const;
```

> You can align initializer list elements by putting the ":" on the next line :

```cpp
DIBO (uint32_t commandCount, GLenum usage)
	: m_commandCount {commandCount}
	, m_usage {usage}
{
	
}
```

> Keywords, and notably ```const``` are on the right side of a name:
```cpp
void onUpdate() const;
Entity::Entity(Entity const rhs);
Entity::Entity(Entity const & rhs);

// it makes it clearer for both of these cases:
T * clone(T const * rhs); // ptr to T const
T * clone(T * const rhs); // const ptr to T
```

> If you see stuff that breaks convention in the codebase, you can fix it yourself.