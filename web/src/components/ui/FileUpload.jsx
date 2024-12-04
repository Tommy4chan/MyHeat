const FileUpload = ({ onChange, className = '', accept = '*', multiple = false }) => {
    return (
      <input
        className={`px-2 py-1 text-white rounded focus:outline-none focus:ring transition duration-300 bg-gray-700 hover:bg-gray-800 focus:ring-gray-800 cursor-pointer file:mr-2 file:py-1 file:px-2 file:rounded file:border-none file:bg-gray-800 file:text-white file:hover:bg-gray-700 ${className}`}
        type="file"
        onChange={onChange}
        accept={accept}
        multiple={multiple}
      />
    );
  };
  
  export default FileUpload;
  