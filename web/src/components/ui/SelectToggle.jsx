import React from 'react';
import BasicSelect from './Select';

const SelectToggle = ({ label, value, onChange, className, color = 'gray' }) => {
  const options = [
    { value: 'false', text: 'Ні' },
    { value: 'true', text: 'Так' }
  ];

  return (
      <BasicSelect
        options={options}
        value={value}
        onChange={onChange}
        className={className}
        color={color}
      />
  );
};

export default SelectToggle;