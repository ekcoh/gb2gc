#ifndef GB2GC_DATASET_H
#define GB2GC_DATASET_H

#include "variant.h"

#include <vector>
#include <type_traits>

namespace gb2gc
{
   template<typename T, typename = void>
   struct is_iterator
   {
      static constexpr bool value = false;
   };

   template<typename T>
   struct is_iterator<T, typename std::enable_if<
         !std::is_same<typename std::iterator_traits<T>::value_type, void>::value
      >::type>
   {
      static constexpr bool value = true;
   };

	class data_set
	{
	public:
		struct column_type;

		using data_type = std::vector<column_type>;
		using column_iterator = data_type::iterator;
		using const_column_iterator = data_type::const_iterator;

      std::vector<std::string> series() const
      {
         std::vector<std::string> series;
         series.reserve(cols());
         for (auto& col : columns())
            series.emplace_back(col.name());
         return series;
      }

		struct column_type
		{
			column_type(size_t rows = 0, std::string name = std::string()) : 
				name_(std::move(name)), data_(rows) 
			{}
			~column_type() noexcept = default;
			column_type(const column_type&) = default;
			column_type& operator=(const column_type&) = default;
			column_type(column_type&& other) = default;
			column_type& operator=(column_type&& other) = default;

			variant& operator[](size_t index) { return data_[index]; }
			const variant& operator[](size_t index) const { return data_[index]; }
			size_t size() const { return data_.size(); }
			size_t rows() const { return size(); }
			bool empty() const { return data_.empty(); }
			void reserve(size_t rows) { data_.reserve(rows); }
			const std::string& name() const { return name_; }
         void set_name(const std::string& name) { name_ = name; }
		private:
			void add_row(variant value = variant{}) { data_.emplace_back(std::move(value)); }
			void resize(size_t rows) { data_.resize(rows); }

			std::string name_;
			std::vector<variant> data_;

			friend class data_set;
		};

		struct const_row_value_iterator
		{
			using value_type = variant;

			explicit const_row_value_iterator(const data_set* ds, size_t col, size_t row) :
				ds_(ds), col_(col), row_(row)
			{ }

			const variant& operator->() const 
			{
				return ds_->get_col(col_)[row_];
			}

			const variant& operator*() const
			{
				return ds_->get_col(col_)[row_];
			}

			const_row_value_iterator& operator++()
			{
				++col_; return *this;
			}

			bool operator==(const const_row_value_iterator& other) const
			{
				if (&ds_ != &other.ds_ && row_ != other.row_)
					return false; // not comparable
				return col_ == other.col_;
			}

			bool operator!=(const const_row_value_iterator& other) const 
			{ 
				bool equal = (*this == other);
				return !equal; 
			}

		private:
			const data_set* ds_;
			size_t col_;
			size_t row_;
		};

		struct row_value_iterator
		{
			using value_type = variant;

			explicit row_value_iterator(data_set* ds, size_t col, size_t row) :
				ds_(ds), col_(col), row_(row)
			{ }

         variant& operator->() const
			{
				return ds_->get_col(col_)[row_];
			}
			
         variant& operator*() const
			{
				return ds_->get_col(col_)[row_];
			}
			
         row_value_iterator& operator++()
			{
				++col_; return *this;
			}

			bool operator==(const row_value_iterator& other) const
			{
				return &ds_ == &other.ds_ && row_ == other.row_ && col_ == other.col_;
			}

			bool operator!=(const row_value_iterator& other) const 
         { 
            return !(*this == other); 
         }

		private:
			data_set* ds_;
			size_t col_;
			size_t row_;
		};

		struct const_row_iterator
		{
			using value_type = const_row_value_iterator;

			const_row_iterator(const data_set* ds, size_t row_index) :
				ds_(ds), ri_(row_index)
			{ }

			value_type operator*() const
			{
				return const_row_value_iterator(ds_, 0, ri_);
			}

			const_row_value_iterator begin() const 
			{
				return const_row_value_iterator(ds_, 0, ri_);
			}

			const_row_value_iterator end() const
			{
				return const_row_value_iterator(ds_, ds_->cols(), ri_);
			}

			const variant& operator[](size_t column_index) const
			{
				return ds_->columns_[column_index][ri_];
			}

			const_row_iterator operator++()
			{
				++ri_;
				return *this;
			}

			bool operator==(const const_row_iterator& other) const
			{
				return ds_ == other.ds_ && ri_ == other.ri_;
			}

			bool operator!=(const const_row_iterator& other) const
			{
				return !(*this == other);
			}

			bool operator<(const const_row_iterator& other) const
			{
				return ds_ == other.ds_ && ri_ < other.ri_;
			}

			bool operator<=(const const_row_iterator& other) const
			{
				return ds_ == other.ds_ && ri_ <= other.ri_;
			}

			bool operator>(const const_row_iterator& other) const
			{
				return ds_ == other.ds_ && ri_ > other.ri_;
			}

			bool operator>=(const const_row_iterator& other) const
			{
				return ds_ == other.ds_ && ri_ >= other.ri_;
			}

		private:
			const data_set* ds_;
			size_t ri_;
		};

		struct row_iterator
		{
			using value_type = row_value_iterator;

			row_iterator(data_set* ds, size_t row_index) :
				ds_(ds), ri_(row_index) 
			{ }

			value_type operator*() const
			{
				return row_value_iterator(ds_, 0, ri_);
			}

			const variant& operator[](size_t column_index) const
			{
				return ds_->columns_[column_index][ri_];
			}

			variant& operator[](size_t column_index)
			{
				return ds_->columns_[column_index][ri_];
			}

			row_iterator operator++()
			{
				++ri_;
				return *this;
			}

			bool operator==(const row_iterator& other) const
			{
				return ds_ == other.ds_ && ri_ == other.ri_;
			}

			bool operator!=(const row_iterator& other) const
			{
				return !(*this == other);
			}

			bool operator<(const row_iterator& other) const
			{
				return ds_ == other.ds_ && ri_ < other.ri_;
			}

			bool operator<=(const row_iterator& other) const
			{
				return ds_ == other.ds_ && ri_ <= other.ri_;
			}

			bool operator>(const row_iterator& other) const
			{
				return ds_ == other.ds_ && ri_ > other.ri_;
			}

			bool operator>=(const row_iterator& other) const
			{
				return ds_ == other.ds_ && ri_ >= other.ri_;
			}

		private:
			data_set* ds_;
			size_t ri_;
		};

		const_row_iterator row_begin() const 
      { 
         return const_row_iterator(this, 0); 
      }

		const_row_iterator row_end() const 
      { 
         return const_row_iterator(this, rows()); 
      }

		row_iterator row_begin() 
      { 
         return row_iterator(this, 0); 
      }

		row_iterator row_end() 
      { 
         return row_iterator(this, rows()); 
      }

		column_iterator col_begin() 
      { 
         return columns_.begin(); 
      }
		
      column_iterator col_end() 
      { 
         return columns_.end(); 
      }

		const_column_iterator col_begin() const 
      { 
         return columns_.begin(); 
      }

		const_column_iterator col_end() const
      { 
         return columns_.end(); 
      }

		row_value_iterator row_begin(size_t row) 
      { 
         return row_value_iterator(this, 0, row); 
      }

		row_value_iterator row_end(size_t row) 
      { 
         return row_value_iterator(this, cols(), row); 
      }

		size_t cols() const noexcept 
      { 
         return columns_.size(); 
      }

		size_t rows() const noexcept 
      { 
         return columns_.empty() ? 0 : columns_[0].rows(); 
      }

		const data_type& data() const noexcept 
      { 
         return columns_; 
      }

		bool empty() const noexcept 
      { 
         return columns_.empty() || columns_[0].empty(); 
      }

		void resize(size_t cols, size_t rows)
		{
			resize_cols(cols);
			resize_rows(rows);
		}

		void resize_cols(size_t cols)
		{
			columns_.resize(cols);
		}

		void resize_rows(size_t rows)
		{
			for (auto& col : columns_)
				col.resize(rows);
		}

		void reserve_cols(size_t cols)
		{
			columns_.reserve(cols);
		}

		void reserve_rows(size_t rows)
		{
			for (auto& col : columns_)
				col.reserve(rows);
		}

		const column_type& get_col(size_t index) const
		{
			return columns_[index];
		}

		const data_type& columns() const 
      { 
         return columns_; 
      }

		column_type& get_col(size_t index)
		{
			return columns_[index];
		}

		void add_column()
		{
			add_column("C" + std::to_string(cols()));
		}

		void add_column(const std::string& name)
		{
			columns_.emplace_back(std::move(column_type{ rows(), name }));
		}

		void add_row()
		{
			const auto m = cols();
			for (auto i = 0u; i < m; ++i)
			{
				get_col(i).add_row();
			}
		}

		template<class FwIt, typename std::enable_if_t<is_iterator<FwIt>::value, int> = 0>
		void add_row(FwIt begin, FwIt end)
		{
			for (auto cit = col_begin(); cit != col_end(); ++cit)
			{
				if (begin != end)
				{
					cit->add_row(*begin);
					++begin;
				}
				else
				{
					cit->add_row();
				}
			}
		}

		template<class... Args>
		void add_row(Args&&... args)
		{
			add_row_item<Args...>(0, std::forward<Args>(args)...);
		}

		template<class T>
		void add_row_value(size_t col_index, T&& value)
		{
			for (auto i = 0u; i < col_index; ++i)
				get_col(i).add_row();
			get_col(col_index).add_row(value);
			const auto m = cols();
			for (auto i = col_index + 1; i < m; ++i)
				get_col(i).add_row();
		}

		template<class FwIt, typename std::enable_if_t<is_iterator<FwIt>::value, int> = 0>
		void add_row_values(size_t first_col_index, FwIt begin, FwIt end)
		{
			for (auto cit = col_begin() + first_col_index; cit != col_end(); ++cit)
			{
				if (begin != end)
				{
					cit->add_row(*begin);
					++begin;
				}
				else
				{
					cit->add_row();
				}
			}
		}

      // TODO Consider implementing sort(size_t col_index) 

	private:
		template<class T, class... Args>
		void add_row_item(size_t index, T&& v, Args&&... args)
		{
			get_col(index).add_row(std::forward<T&&>(v));
			add_row_item(index + 1, std::forward<Args>(args)...);
		}

		template<class T>
		void add_row_item(size_t index, T&& v)
		{
			get_col(index).add_row(std::forward<T&&>(v));
			const auto m = cols();
			for (auto i = index + 1; i < m; ++i)
				get_col(i).add_row();
		}

		data_type columns_;
	};

	std::ostream& operator<<(std::ostream& os, const ::gb2gc::data_set::const_row_iterator& rit);
	std::ostream& operator<<(std::ostream& os, const ::gb2gc::data_set& ds);
		
} // namespace gb2gc

#endif // GB2GC_DATASET_H