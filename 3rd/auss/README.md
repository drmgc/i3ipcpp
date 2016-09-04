# AutoStringStream ![license](https://img.shields.io/npm/l/chas-storage.svg)

Simple header-only wrapper on `std::stringstream` with automatic casting to `std::string`

## Usage

```c++
#include <auss.hpp>
```
```c++
auss_t() << "Hello, " << user_name
```
```c++
throw std::runtime_error(auss_t() << "Something gone wrong, See " << log_path)
```

### Own namespace
If you wouldn't pollute global namespace just define `AUSS_USE_OWN_NAMESPACE`. Either before `#include` or in compiler flags (`-DAUSS_USE_OWN_NAMESPACE` for GCC).

Also you can specifiy the name of namespace with `AUSS_OWN_NAMESPACE_NAME`:
```
-DAUSS_OWN_NAMESPACE_NAME="theauss"
```

## License

Licensed under Unlicense. See `LICENSE` file for more info.
