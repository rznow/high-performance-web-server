#ifndef BUFFER_H
#define BUFFER_H

class Buffer
{
    private:
        std::string buffer;
    public:
        size_t size();
        bool empty();
        void append(const char* s, size_t len);
        void append(const std::string& s);
        void retrieve(size_t len);
        void retrieveAll();
        const std::string& data() const;
};
#endif