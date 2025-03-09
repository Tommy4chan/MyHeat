import React from 'react';
import { toast } from 'react-toastify';
import { MdError, MdWarning, MdCheck, MdInfo } from "react-icons/md";

const STATUS_TYPE = {
  error: 'error',
  warning: 'warning',
  success: 'success',
  info: 'info',
}

const CustomToast = ({ type, message, closeToast }) => {
  const getToastStyles = () => {
    switch (type) {
      case STATUS_TYPE.error:
        return {
          iconClass: 'text-red-500',
          icon: <MdError size={25} />,
          title: 'Помилка',
          borderColor: 'border-red-500'
        };
      case STATUS_TYPE.warning:
        return {
          iconClass: 'text-yellow-500',
          icon: <MdWarning size={25} />,
          title: 'Попередження',
          borderColor: 'border-yellow-500'
        };
      case STATUS_TYPE.success:
        return {
          iconClass: 'text-green-500',
          icon: <MdCheck size={25} />,
          title: 'Успішно',
          borderColor: 'border-green-500'
        };
      default:
        return {
          iconClass: 'text-blue-500',
          icon: <MdInfo size={25} />,
          title: 'Інформація',
          borderColor: 'border-blue-500'
        };
    }
  };

  const styles = getToastStyles();

  return (
    <div className={`flex items-start p-4 bg-gray-800 border ${styles.borderColor} rounded-lg shadow-lg w-full`}>
      <div className={`flex-shrink-0 mr-3 ${styles.iconClass} text-xl font-bold flex items-center justify-center w-6 h-6`}>
        {styles.icon}
      </div>
      <div className="flex-1">
        <h3 className="font-semibold text-lg text-white mb-1">
          {styles.title}
        </h3>
        <p className="text-gray-300">
          {message}
        </p>
      </div>
      <button 
        onClick={closeToast}
        className="absolute top-2 right-2 text-gray-300 hover:text-white p-1"
      >
        ✕
      </button>
    </div>
  );
};

const showToast = (type, message) => {
  toast(<CustomToast type={type} message={message} />, {
    position: "top-right",
    hideProgressBar: true,
    autoClose: type == STATUS_TYPE.error ? false : 5000,
    closeButton: false,
    closeOnClick: true,
    pauseOnHover: true,
    draggable: false,
    className: 'p-0 bg-transparent',
  });
};

export const showSuccessToast = (message) => {
  showToast(STATUS_TYPE.success, message);
};

export const showErrorToast = (message) => {
  showToast(STATUS_TYPE.error, message);
}

export const showInfoToast = (message) => {
  showToast(STATUS_TYPE.info, message);
}

export const showWarningToast = (message) => {
  showToast(STATUS_TYPE.warning, message);
}

export default showToast;