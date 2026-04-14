/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   libft.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ctommasi <ctommasi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/09/16 16:41:22 by ctommasi          #+#    #+#             */
/*   Updated: 2026/04/14 13:22:50 by ctommasi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LIBFT_H
# define LIBFT_H

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 100
# endif

# include <unistd.h>
# include <stdlib.h>
# include <fcntl.h>
# include <stdio.h>
# include <stddef.h>
# include <stdarg.h>

typedef struct s_list
{
	void			*content;
	int				value;
	int				index;
	struct s_list	*next;
	struct s_list	*prev;
}	t_list;

/// @brief Returns 1 if char is A - Z / a - z.
int		ft_isdigit(int c);
/// @brief Returns 1 if char 0 - 9 / A - Z / a - z.
int		ft_isalnum(int c);
/// @brief Returns 1 if char is ASCII.
int		ft_isascii(int c);
/// @brief Returns 1 if char is a printable char.
int		ft_isprint(int c);
/// @brief Returns length of string, not including '\0'.
size_t	ft_strlen(const char *s);
/// @brief Fills n bytes of the memory area pointed to by s
/// @brief with the constant byte c.
void	*ft_memset(void *s, int c, size_t n);
/// @brief Erases the data in the n bytes of the memory starting
/// @brief at the location pointed to by s, by writing '\0'.
void	ft_bzero(void *s, size_t n);
/// @brief Copies n bytes from memory area src to memory area dest. 
/// @brief The memory areas must not overlap.
void	*ft_memcpy(void *dest, const void *src, size_t n);
/// @brief Copies n bytes from memory area src to memory area dest.  
/// @brief The memory areas may overlap.
void	*ft_memmove(void *dest, const void *src, size_t n);
/// @brief Copies up to size - 1 characters from the NUL-terminated
/// @brief string src to dst, NUL-terminating the result.
size_t	ft_strlcpy(char *dst, const char *src, size_t size);
/// @brief Appends  the NUL-terminated string src to the end of dst. 
/// @brief It will append at most size - strlen(dst) - 1 bytes,
/// @brief NUL-terminating the result.
size_t	ft_strlcat(char *dst, const char *src, size_t size);
/// @brief Converts lowercap to uppercap.
int		ft_toupper(int ch);
/// @brief Converts uppercap to lowercap.
int		ft_tolower(int ch);
/// @brief Returns a pointer to the first occurrence of the character 
/// @brief c in the string s. Else returns 0.
char	*ft_strchr(const char *s, int c);
/// @brief Returns a pointer to the last occurrence of the character 
/// @brief c in the string s.
char	*ft_strrchr(const char *s, int c);
/// @brief Compares the two strings s1 and s2 up to n bytes. Returns 0 
/// @brief if equal, negative value if s1 < s2, positive value if s1 > s2.
int		ft_strncmp(const char *s1, const char *s2, size_t n);
/// @brief Scans the initial n bytes of the memory area pointed to by s
/// @brief for the first instance of c. Both c and the bytes of the 
/// @brief memory area pointed to by s are interpreted as unsigned char.
void	*ft_memchr(const void *s, int c, size_t n);
/// @brief returns an integer less than, equal to, or greater than zero 
/// @brief if the first n bytes of s1 is found, respectively, to be less 
/// @brief than, to match, or be greater than the first n bytes of s2.
int		ft_memcmp(const void *s1, const void *s2, size_t n);
/// @brief Finds the first occurrence  of the substring 
/// @brief little in the string big. 
char	*ft_strnstr(const char *big, const char *little, size_t len);
/// @brief Finds the first occurrence up to n bytes of the substring 
/// @brief little in the string big. 
char	*ft_strstr(char *str, char *to_find);
/// @brief Converts the initial portion of the string pointed to 
/// @brief by nptr to long.
long	ft_atoi(const char *nptr);
/// @brief Allocates memory and sets it to 0 for an array of nmemb elements 
/// @brief of size bytes each and returns a pointer to the  allocated  memory.
void	*ft_calloc(size_t nmemb, size_t size);
/// @brief Returns a pointer to a new string allocated 
/// @brief which is a duplicate of the string s.
char	*ft_strdup(const char *s);
/// @brief Extracts a substring from start to len.
char	*ft_substr(char const *s, unsigned int start, size_t len);
/// @brief Returns a new string of s1 and s2 concatenated.
char	*ft_strjoin(char const *s1, char const *s2);
/// @brief Returns a new allocated string which is s1 trimmed by set.
char	*ft_strtrim(char const *s1, char const *set);
/// @brief Split a string by char into a array of strings.
char	**ft_split(char const *s, char c);
/// @brief Converts an int into a str.
char	*ft_itoa(int n);
/// @brief Apply the function 'f' to each characters in 
/// @brief the string 's' to create a new string 
/// @brief resulting of the successive applications of 'f'.
char	*ft_strmapi(char const *s, char (*f)(unsigned int, char));
/// @brief Apply the function 'f' to each characters of the string 's', passing 
/// @brief its index as a first parameter.
void	ft_striteri(char *s, void (*f)(unsigned int, char *));
/// @brief The ft_putchar_fd() function writes the character 
/// @brief c on the file descriptor fd.
void	ft_putchar_fd(char c, int fd);
/// @brief Write the string s on the file descriptor fd.
void	ft_putstr_fd(char *s, int fd);
/// @brief Writes the string s, followed by a newline, on 
/// @brief the file descriptor fd
void	ft_putendl_fd(char *s, int fd);
/// @brief Writes the int n on the file descriptor fd.
void	ft_putnbr_fd(int n, int fd);
/// @brief Trims str by chars. No allocation.
char	*ft_strdelchar(const char *str, const char *chars);
/// @brief Changes the size of the memory block pointed to by ptr to size bytes.
void	*ft_realloc(void *ptr, size_t cur_size, size_t new_size);
/// @brief Trims str by chars. No allocation.
void	ft_ptrdelchar(char *str, const char *chars);
/// @brief  Add the 'new' element at the front of the list.
void	ft_lstadd_front(t_list **lst, t_list *neww);
/// @brief Return size of a singly linked list.
int		ft_lstsize(t_list *lst);
/// @brief Free the memory of the element passed as parameter using 
/// @brief the 'del' function then free. 
/// @brief The memory of 'next' must not be freed.
void	ft_lstdelone(t_list *lst, void (*del)(void*));
/// @brief Deletes and free the memory of the element passed 
/// @brief as parameter and all the following elements using 'del' and free(3).
/// @brief Finally, the initial pointer must be set to NULL.
void	ft_lstclear(t_list **lst, void (*del)(void *));
/// @brief Iterate over the list 'lst' and apply the function 
/// @brief 'f' to the content of all elements.
void	ft_lstiter(t_list *lst, void (*f)(void *));
/// @brief Iterate over the list 'lst' and apply the function 
/// @brief 'f' to the content of each elements. Create a new list 
/// @brief resulting of the successive applications of 'f'. The 
/// @brief function 'del' is used to destroy the content of 
/// @brief an element if necessary.
t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *));
/// @brief Compares the two strings s1 and s2. Returns 0 if equal, 
/// @brief negative value if s1 < s2, positive value if s1 > s2.
int		ft_strcmp(const char *s1, const char *s2);
/// @brief Returns 1 if linked list has duplicate elements.
int		ft_lstcheck_dupe(t_list **stack_x);
/// @brief Returns 1 if linked list is ordered in ascending order.
int		ft_lst_isordered(t_list **stack_x);
/// @brief Returns the length of a linked list.
size_t	ft_lstlen(t_list **stack_x);
/// @brief Created a new node with malloc for a singly linked list.
t_list	*ft_lstnew(int value);
/// @brief Returns the node with the max index in a singly linked list.
t_list	*ft_lstmax_index(t_list **stack_x);
/// @brief Returns the last element of the list.
t_list	*ft_lstlast(t_list **stack_x);
/// @brief Add the 'new' element at the back of the list.
void	ft_lstadd_back(t_list **lst, t_list *neww);
/// @brief Compares each element to one another and assigns 
/// @brief indices based on thei relative position.
void	ft_lstinit_index(t_list **stack_x);
/// @brief Free a singly linked list.
void	ft_lstfree(t_list **stack_x);
/// @brief Accepts an fd and returns the next line of file 
/// @brief on each call. Statically saved.

#endif
