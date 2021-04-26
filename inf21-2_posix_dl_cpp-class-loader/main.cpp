#include <string>
#include <regex>

#include <dlfcn.h>
#include <cerrno>


#include <string>

struct ClassImpl {
    void *constructor_ptr = nullptr;
};

class AbstractClass {
    friend class ClassLoader;

public:
    explicit AbstractClass();

    ~AbstractClass();

protected:
    void *newInstanceWithSize(size_t sizeofClass);

    struct ClassImpl *pImpl;
};

template<class T>
class Class
        : public AbstractClass {
public:
    T *newInstance() {
        size_t classSize = sizeof(T);
        void *rawPtr = newInstanceWithSize(classSize);
        return reinterpret_cast<T *>(rawPtr);
    }
};

enum class ClassLoaderError {
    NoError = 0,
    FileNotFound,
    LibraryLoadError,
    NoClassInLibrary
};

struct ClassLoaderImpl {
    ClassLoaderError error;
};


class ClassLoader {
public:
    explicit ClassLoader();

    AbstractClass *loadClass(const std::string &fullyQualifiedName);

    ClassLoaderError lastError() const;

    ~ClassLoader();

private:
    struct ClassLoaderImpl *pImpl;
};


// implementation:

AbstractClass::AbstractClass() {
    pImpl = new ClassImpl();
}

AbstractClass::~AbstractClass() {
    delete pImpl;
}

void *AbstractClass::newInstanceWithSize(size_t sizeofClass) {
    void *class_ptr = malloc(sizeofClass);

    typedef void (*constructor_t)(void *);
    auto constructor = (constructor_t) pImpl->constructor_ptr;
    constructor(class_ptr);
    return class_ptr;
};


std::string getConstructorMangledName(const std::string &className) {
    std::string name = "_ZN";
    for (int i = 0; i < className.size(); ++i) {
        char c = className[i];
        int name_len = 0;
        if (c != ':') {
            while (i < className.size() && className[i] != ':') {
                ++name_len, ++i;
            }
            name += std::to_string(name_len);
            name += className.substr(i - name_len, name_len);
        }
    }
    name += "C1Ev";
    return name;
}


AbstractClass *ClassLoader::loadClass(const std::string &fullyQualifiedName) {
    // Базовый каталог: CLASSPATH
    const std::string classpath = std::getenv("CLASSPATH");
    std::string filename = std::regex_replace(fullyQualifiedName, std::regex("::"), "/");
    const std::string filepath = classpath + "/" + filename + ".so";
    auto *abstractClass = new AbstractClass();
    void *lib = dlopen(filepath.c_str(), RTLD_NOW | RTLD_GLOBAL);
    if (!lib) {
        if (errno == ENOENT) {
            pImpl->error = ClassLoaderError::FileNotFound;
        } else {
            pImpl->error = ClassLoaderError::LibraryLoadError;
        }
        return nullptr;
    }

    std::string constructorName = getConstructorMangledName(fullyQualifiedName);
    void *constructor_sym = dlsym(lib, constructorName.c_str());
    if (constructor_sym == nullptr) {
        pImpl->error = ClassLoaderError::NoClassInLibrary;
        return nullptr;
    }

    abstractClass->pImpl->constructor_ptr = constructor_sym;

    return abstractClass;
}


ClassLoader::ClassLoader() {
    pImpl = new ClassLoaderImpl();
    pImpl->error = ClassLoaderError::NoError;
}

ClassLoaderError ClassLoader::lastError() const {
    return pImpl->error;
}

ClassLoader::~ClassLoader() {
    delete pImpl;
}
