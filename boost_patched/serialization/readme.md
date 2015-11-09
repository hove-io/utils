Uggly copy of boost file because the vector serialization is buggy in boost 1.58 (and has been corrected in boost 1.59)

what is missing is the serialization of non default constructible objects in the vector

cf: http://lists.boost.org/boost-users/2015/06/84377.php
