#ifndef BLOCK_CONFIG_HPP
# define BLOCK_CONFIG_HPP

class BlockConfig
{
	public:
		BlockConfig();
		BlockConfig(BlockConfig const &src);
		virtual ~BlockConfig();

		BlockConfig &					operator=(BlockConfig const &src);
		
		bool							isAutoIndex() const
		{
			return (this->_autoindex);
		}

		int								getBodySize() const
		{
			return (this->_body_size);
		}

		bool							isValueSet(std::string key)
		{
			return (this->_values_set[key] == true);
		}

		bool							isValueSet(std::string key) const
		{
			return (this->_values_set.find(key)->second == true);
		}

		std::string						getRoot() const
		{
			return (this->_root);
		}

		std::string						getIndex() const
		{
			return (this->_index);
		}

		std::map<std::string, bool> &	getMethodsAllowed()
		{
			return (this->_methods_allowed);
		}

		std::map<std::string, bool> 	getMethodsAllowed() const
		{
			return (this->_methods_allowed);
		}

		std::map<std::string, bool> &	getCgiExtensions()
		{
			return (this->_cgi_extensions);
		}

		std::map<std::string, bool>		getCgiExtensions() const
		{
			return (this->_cgi_extensions);
		}

		bool							isCgiExtension(std::string extension) const
		{
			return (this->_cgi_extensions.find(extension)->second == true);
		}

		std::map<int, std::string>	getErrorPages() const
		{
			return (this->_error_pages);
		}

		void							addErrorPage(int error_code, std::string path)
		{
			this->_error_pages[error_code] = path;
		}

		void							addCgiExtension(std::string extension)
		{
			this->_cgi_extensions[extension] = true;
		}

		void							setRoot(std::string root)
		{
			this->_root = root;
		}

		void							setIndex(std::string index)
		{
			this->_index = index;
		}

		void							setValue(std::string key, bool set)
		{
			this->_values_set[key] = set;
		}

		void							setBodySize(int body_size)
		{
			this->_body_size = body_size;
		}

		void							setAutoIndex(bool autoindex)
		{
			this->_autoindex = autoindex;
		}

		void							setMethodsAllowed(std::map<std::string, bool> methods_allowed)
		{
			this->_methods_allowed = methods_allowed;
		}

		void							setErrorPages(std::map<int, std::string> error_pages)
		{
			this->_error_pages = error_pages;
		}

		void							setCgiExtensions(std::map<std::string, bool> cgi_extensions)
		{
			this->_cgi_extensions = cgi_extensions;
		}
	protected:
		std::map<std::string, bool>				_values_set;
		std::map<std::string, bool>				_methods_allowed;
		bool 									_autoindex;
		int										_body_size;
		std::string								_root;
		std::string								_index;
		std::map<int, std::string>				_error_pages;
		std::map<std::string, bool>				_cgi_extensions;
};

#endif