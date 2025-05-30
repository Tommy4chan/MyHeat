import React from 'react';
import BasicSelect from './Select';

const SelectToggle = ({ value, onChange, className, color = 'gray', name = '' }) => {
  const options = [
    { value: false, text: 'Ні' },
    { value: true, text: 'Так' }
  ];

  return (
      <BasicSelect
        options={options}
        value={value}
        onChange={onChange}
        className={className}
        color={color}
        name={name}
      />
  );
};

export default SelectToggle;