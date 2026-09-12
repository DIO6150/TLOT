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

## Brackets

Curly brackets must be on the next line :
```cpp
class AssetManager
{
public:
	void OnUpdate()
	{
		if (alive_ == true)
		{

		}
	};
};
```

Opening Brackets must glue to the previous character unless its from a control block keyword :
```cpp
void OnUpdate()
{
	if (alive_ == true)
	{
		while (alive_)
		{
			currentHp_ -= 1;
			UpdateAliveStatus();
		}
	}
}
```

## Miscellanious

> Try to be as "airy" as possible. Less clutter means more times spent on finding what you need.

> Try to align stuff that repeat on multiple lines by putting appropriate number of spaces between tokens :

```cpp
m_geometry  .Create(vertexTemplate);
m_matrixSSBO.Create(GL_DYNAMIC_DRAW, 1, 10000000);
m_indexSSBO .Create(GL_DYNAMIC_DRAW, 0, 10000000);
```

> However it does not look good when aligning functions declarations

```cpp
void UploadMatrix4(std::string key, const glm::mat4& matrix) const;
void Upload1i     (std::string key, int i) const;
void Upload3fv    (std::string key, glm::vec3 vec) const;
void Upload1f     (std::string key, float f) const;
// ugly af :(
```

> You can align initializer list elements by putting the ":" on the next line :

```cpp
DIBO (uint32_t commandCount, GLenum usage)
	: m_commandCount {commandCount}
	, m_usage {usage}
{
	
}
```

> If you see stuff that breaks convention in the codebase, you can fix it yourself