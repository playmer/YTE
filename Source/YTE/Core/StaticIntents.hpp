#pragma once

/* Static Intents */
// For file specific static variables or functions
#define intern        static
// For global static variables
#define global          static
// For having variables that maintain state through method or function calls
#define local_persist   static
// For static variables shared between methods in classes or structs
#define shared_instance static
// For marking a static function in a class or struct with no this pointer, that isn't a friend
#define non_member      static