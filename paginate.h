#pragma once
namespace navitia {
template<typename T>
std::vector<T> paginate(const std::vector<T> &indexes,
                                  int count, int start_page) {
    std::vector<T> response;
    if(count>=0 && start_page>=0) {
        uint32_t begin_i = start_page * count;
        uint32_t end_i = begin_i + count;
        if(begin_i < indexes.size()) {
            auto begin = indexes.begin() + begin_i;
            auto end = (end_i < indexes.size()) ? indexes.begin() + end_i :
                                                  indexes.end();
            return std::vector<T>(begin, end);
        }
    }
    return std::vector<T>();
}

}
